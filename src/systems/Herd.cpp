#include "Herd.h"
#include "../core/Text.h"
#include <algorithm>

namespace {
constexpr float speed = 2.4f, gap = 5;
// Follow the pasture gate and the existing paths, then the barn's central aisle.
const std::array<Vec3, 5> lane{{{7, 0, 15}, {0, 0, 15}, {0, 0, 0}, {-17, 0, 0}, Barn::aisle(1)}};
Vec3 queuePosition(int slot) {
    return {7 + float(slot) * gap, 0, 15};
}
template <size_t N> float routeLength(const std::array<Vec3, N>& points) {
    float result = 0;
    for (size_t i = 1; i < N; ++i)
        result += length(points[i] - points[i - 1]);
    return result;
}
template <size_t N> Vec3 sample(const std::array<Vec3, N>& points, float distance) {
    for (size_t i = 1; i < N; ++i) {
        Vec3 segment = points[i] - points[i - 1];
        float size = length(segment);
        if (size < .0001f)
            continue;
        if (distance <= size)
            return points[i - 1] + segment * (distance / size);
        distance -= size;
    }
    return points.back();
}
template <size_t N>
void follow(Cow& cow, const std::array<Vec3, N>& points, float distance, float dt) {
    // Look ahead into corners so the model turns before its centre reaches the bend.
    cow.follow(sample(points, distance),
               sample(points, distance + .8f) - sample(points, distance - .15f), dt);
}
std::array<Vec3, 3> stallRoute(int cow) {
    return {lane.back(), Barn::aisle(cow), Barn::bed(cow)};
}
std::array<Vec3, 7> exitRoute(int cow, int slot) {
    return {Barn::bed(cow), Barn::aisle(cow), lane.back(),        lane[3],
            lane[2],        lane[1],          queuePosition(slot)};
}
} // namespace

Herd::Herd() {
    for (auto& cow : cows)
        cow.update(0);
}
void Herd::update(float dt, bool night, Barn& barn) {
    if (!std::isfinite(dt) || dt <= 0)
        return;
    // Bounded steps also make large capture/test time advances obey door and queue rules.
    while (dt > .000001f) {
        float slice = std::min(dt, 1.0f / 60);
        step(slice, night, barn);
        dt -= slice;
    }
}
void Herd::step(float dt, bool night, Barn& barn) {
    if (state == Phase::Grazing && night) {
        for (size_t i = 0; i < cows.size(); ++i)
            fieldPositions[i] = cows[i].position();
        std::stable_sort(order.begin(), order.end(),
                         [&](int a, int b) { return fieldPositions[a].x < fieldPositions[b].x; });
        state = Phase::Gathering;
    }
    if (state == Phase::Gathering && !night)
        state = Phase::Returning;
    if (state == Phase::Sleeping && !night) {
        exitProgress.fill(0);
        turnedFromStall.fill(false);
        turnedToDoor.fill(false);
        state = Phase::Leaving;
    }
    // During barn traffic, finish the current direction. Only the latest target is
    // retained, so repeated toggles cannot reset paths, teleport cows or grow a queue.
    barn.update(dt,
                state == Phase::Gathering || state == Phase::Entering || state == Phase::Leaving);
    switch (state) {
    case Phase::Grazing:
        fieldTime += dt;
        for (auto& cow : cows)
            cow.update(fieldTime);
        break;
    case Phase::Gathering: {
        bool ready = true;
        for (int slot = 0; slot < 3; ++slot)
            ready &= cows[order[slot]].walkTo(queuePosition(slot), dt);
        if (ready) {
            convoyDistance = stallDistance = 0;
            nextCow = 0;
            enteringStall = false;
            state = Phase::Entering;
        }
        break;
    }
    case Phase::Entering:
        enter(dt, barn);
        break;
    case Phase::Sleeping:
        for (auto& cow : cows)
            cow.rest(true, dt);
        break;
    case Phase::Leaving:
        leave(dt, barn);
        break;
    case Phase::Returning: {
        bool ready = true;
        for (size_t i = 0; i < cows.size(); ++i)
            ready &= cows[i].walkTo(fieldPositions[i], dt);
        if (ready)
            state = Phase::Grazing;
        break;
    }
    }
}
void Herd::enter(float dt, const Barn& barn) {
    auto route = stallRoute(order[nextCow]);
    float end = routeLength(route);
    if (enteringStall)
        stallDistance = std::min(end, stallDistance + speed * dt);
    else if (barn.passable()) {
        float stop = routeLength(lane) + float(nextCow) * gap;
        convoyDistance = std::min(stop, convoyDistance + speed * dt);
        if (convoyDistance >= stop) {
            enteringStall = true;
            stallDistance = 0;
        }
    }
    for (int slot = 0; slot < 3; ++slot) {
        Cow& cow = cows[order[slot]];
        if (slot < nextCow)
            cow.rest(true, dt);
        else if (slot == nextCow && enteringStall)
            follow(cow, route, stallDistance, dt);
        else
            follow(cow, lane, convoyDistance - float(slot) * gap, dt);
    }
    if (enteringStall && stallDistance >= end) {
        enteringStall = false;
        if (++nextCow == 3)
            state = Phase::Sleeping;
    }
}
void Herd::leave(float dt, const Barn& barn) {
    bool ready = true;
    // Send the farthest queue member out first; following cows stop behind it.
    for (int rank = 0; rank < 3; ++rank) {
        int slot = 2 - rank, id = order[slot];
        auto route = exitRoute(id, slot);
        float end = routeLength(route);
        bool released = rank == 0;
        if (rank > 0) {
            int previousId = order[slot + 1];
            released = exitProgress[rank - 1] >= routeLength(stallRoute(previousId)) + gap;
        }
        if (barn.passable() && released) {
            float backOut = length(Barn::aisle(id) - Barn::bed(id));
            float toAisle = routeLength(stallRoute(id));
            // Back out facing the hay, then turn in the clear aisle outside the rails.
            if (exitProgress[rank] < backOut) {
                exitProgress[rank] = std::min(backOut, exitProgress[rank] + speed * dt);
                cows[id].follow(sample(route, exitProgress[rank]), {0, 0, -1}, dt);
            } else if (!turnedFromStall[rank]) {
                Vec3 direction = lane.back() - Barn::aisle(id);
                turnedFromStall[rank] = length(direction) < .001f || cows[id].face(direction, dt);
            } else if (exitProgress[rank] < toAisle) {
                exitProgress[rank] = std::min(toAisle, exitProgress[rank] + speed * dt);
                cows[id].follow(sample(route, exitProgress[rank]), lane.back() - Barn::aisle(id),
                                dt);
            } else if (!turnedToDoor[rank])
                turnedToDoor[rank] = cows[id].face({0, 0, 1}, dt);
            else {
                exitProgress[rank] = std::min(end, exitProgress[rank] + speed * dt);
                follow(cows[id], route, exitProgress[rank], dt);
            }
        } else
            cows[id].rest(false, dt);
        ready &= exitProgress[rank] >= end;
    }
    if (ready)
        state = Phase::Returning;
}
int Herd::sleepingCount() const {
    return int(
        std::count_if(cows.begin(), cows.end(), [](const Cow& cow) { return cow.sleeping(); }));
}
std::string Herd::status(bool night) const {
    switch (state) {
    case Phase::Grazing:
        return Text::get("herd.grazing");
    case Phase::Gathering:
        return Text::get("herd.gathering");
    case Phase::Entering:
        return Text::get(night ? "herd.entering" : "herd.entering_day");
    case Phase::Sleeping:
        return Text::get("herd.sleeping");
    case Phase::Leaving:
        return Text::get(night ? "herd.leaving_night" : "herd.leaving");
    case Phase::Returning:
        return Text::get(night ? "herd.returning_night" : "herd.returning");
    }
    return {};
}
void Herd::render() const {
    for (const auto& cow : cows)
        cow.render();
}
