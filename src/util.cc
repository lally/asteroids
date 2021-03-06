
#include "util.h"
#include <deque>
#include <utility>

static bool s_record_bullets = false;
static std::deque<active::ptr> s_bullets;

void util::enable_bullet_recording() {
    s_record_bullets = true;
}

void util::note_new_bullet(active::ptr& ptr) {
    if (s_record_bullets) {
        s_bullets.push_back(ptr);
    }
}

size_t util::get_bullet_ptrs(std::vector<active::ptr>* dest) {
    size_t n = s_bullets.size();
    if (n) {
        std::copy(s_bullets.begin(), s_bullets.end(),
                  back_inserter(*dest));
        s_bullets.clear();
    }
    return n;
}
