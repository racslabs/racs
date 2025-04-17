#include "ogg.h"

void auxts_ogg_set_channels(auxts_ogg* ogg, auxts_uint16 channels) {
    ogg->format.channels = channels;
}

void auxts_ogg_set_sample_rate(auxts_ogg* ogg, auxts_uint32 sample_rate) {
    ogg->format.sample_rate = sample_rate;
}

void auxts_ogg_init(auxts_ogg* ogg) {
    ogg->err = OPE_OK;
    ogg->r_pos = 0;
    ogg->w_pos = 0;

    ogg->comments = ope_comments_create();
    if (!ogg->comments) {
        perror("Failed to create ogg comments\n");
        return;
    }

    ogg->enc = ope_encoder_create_pull(ogg->comments, (int)ogg->format.sample_rate, ogg->format.channels, OPUS_APPLICATION_AUDIO, &ogg->err);
    if (!ogg->enc) {
        fprintf(stderr, "Failed to create ogg encoder: %d\n", ogg->err);
        ope_comments_destroy(ogg->comments);
    }
}

void auxts_ogg_read_pcm_frames(auxts_ogg* ogg, const auxts_int16* in, size_t samples) {
    int samples_per_channel = (int)(0.02 * ogg->format.sample_rate);
    off_t frame_size = samples_per_channel * ogg->format.channels;

    auxts_int16* pcm = malloc(sizeof(auxts_int16) * frame_size);
    if (!pcm) {
        perror("Failed to allocate pcm frame");
        return;
    }

    size_t size = samples * ogg->format.channels;
    while (ogg->r_pos < size) {
        memcpy(pcm, in + ogg->r_pos, frame_size);

        ogg->err = ope_encoder_write(ogg->enc, pcm, samples_per_channel);
        if (ogg->err != OPE_OK) {
            ope_comments_destroy(ogg->comments);
            ope_encoder_destroy(ogg->enc);
            break;
        }

        ogg->r_pos += frame_size;
    }

    free(pcm);
}

