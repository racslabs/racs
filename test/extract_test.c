#include "extract_test.h"

void test_extract() {
    auxts_cache* cache = auxts_cache_create(2);
    auxts_result result = auxts_extract(cache, 15652479119055622818ull, 1739141512213ull, 1739141512213ull);

}
