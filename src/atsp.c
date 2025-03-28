#include "atsp.h"

int auxts_atsp_header_parse(int socket_fd, auxts_atsp_header* header) {
    uint8_t buf[26];

    if (recv(socket_fd, buf, 26, MSG_WAITALL) != 26)
        return false;

    memcpy(&header->chunk_id, buf, 4);
    auxts_read_uint64(&header->hash, buf, 4);
    auxts_read_uint32(&header->checksum, buf, 12);
    auxts_read_uint16(&header->channels, buf, 16);
    auxts_read_uint32(&header->sample_rate, buf, 18);
    auxts_read_uint16(&header->bit_depth, buf, 22);
    auxts_read_uint16(&header->block_size, buf, 24);

    return true;
}
