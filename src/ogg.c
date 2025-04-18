#include "ogg.h"

void auxts_ogg_set_channels(auxts_ogg* ogg, auxts_uint16 channels) {
    ogg->format.channels = channels;
}

void auxts_ogg_set_sample_rate(auxts_ogg* ogg, auxts_uint32 sample_rate) {
    ogg->format.sample_rate = sample_rate;
}

int auxts_ogg_write_callback(void *user_data, const auxts_uint8* ptr, auxts_int32 len) {
    auxts_ogg* ogg = (auxts_ogg*) user_data;
    if (!ogg) {
        perror("auxts_ogg cannot be null");
        return 1;
    }

    memcpy(((auxts_uint8*)ogg->ptr) + ogg->w_pos, ptr, len);
    ogg->w_pos += len;

    return 0;
}

int auxts_ogg_read_callback(void *user_data) {
    (void)user_data;
    return 1;
}

void auxts_ogg_init(auxts_ogg* ogg, void* out) {
    ogg->err   = OPE_OK;
    ogg->r_pos = 0;
    ogg->w_pos = 0;
    ogg->ptr   = out;

    OpusEncCallbacks callbacks = {
            .write = auxts_ogg_write_callback,
            .close = auxts_ogg_read_callback
    };

    ogg->comments = ope_comments_create();
    if (!ogg->comments) {
        perror("Failed to allocate OggOpusComments");
        return;
    }

    ogg->enc = ope_encoder_create_callbacks(&callbacks, ogg, ogg->comments, (int)ogg->format.sample_rate, ogg->format.channels, 0, &ogg->err);
    if (!ogg->enc) {
        perror("Failed to create OggOpusEnc");
        return;
    }
}

size_t auxts_ogg_write(auxts_ogg* ogg, const void* in, void* out, size_t samples, size_t size) {
    auxts_ogg_init(ogg, out);

    int samples_per_channel = (int)(AUXTS_OGG_FRAMESIZE_20MS * ogg->format.sample_rate);
    off_t frame_size = samples_per_channel * ogg->format.channels;

    auxts_int16* pcm = malloc(sizeof(auxts_int16) * frame_size);
    if (!pcm) {
        perror("Failed to allocate pcm frame");
        return 0;
    }

    while (ogg->r_pos < samples * ogg->format.channels) {
        memcpy(pcm, in + ogg->r_pos, frame_size);

        ogg->err = ope_encoder_write(ogg->enc, pcm, samples_per_channel);
        if (ogg->err != OPE_OK) {
            fprintf(stderr, "Filed to encode ogg: %s\n", ope_strerror(ogg->err));
            free(pcm);
            ope_comments_destroy(ogg->comments);
            ope_encoder_destroy(ogg->enc);
            break;
        }

        ogg->r_pos += frame_size;
    }

    free(pcm);
    return ogg->w_pos;
}

size_t auxts_ogg_write_s16(auxts_ogg* ogg, const auxts_int16* in, void* out, size_t samples, size_t size) {
    return auxts_ogg_write(ogg, in, out, samples, size);
}

void auxts_ogg_destroy(auxts_ogg* ogg) {
    ope_encoder_drain(ogg->enc);
    ope_encoder_destroy(ogg->enc);
    ope_comments_destroy(ogg->comments);
}
