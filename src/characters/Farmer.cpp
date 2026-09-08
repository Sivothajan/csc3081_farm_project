#include "Farmer.h"
#include "../utils/Helpers.h"
#include "../vegetation/Crop.h"
#include <algorithm>
#include <iostream>

namespace {
constexpr float speed = 1.12f, stride = 1.32f;
const Vec3 skin{.93f, .79f, .61f}, shirt{.64f, .23f, .14f}, cuff{.79f, .59f, .35f},
    trousers{.12f, .26f, .37f}, leather{.25f, .14f, .07f}, hair{.22f, .14f, .075f};
float smooth(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}
// Elliptical rings make fitted clothing instead of a stack of boxes.
struct Ring {
    float y, width, depth;
};
void tailored(std::initializer_list<Ring> rings, Vec3 color, bool plaid = false) {
    glColor3f(color.x, color.y, color.z);
    auto lower = rings.begin();
    for (auto upper = lower + 1; upper != rings.end(); lower = upper++) {
        int bands = plaid ? std::max(1, int((upper->y - lower->y) / .025f)) : 1;
        for (int band = 0; band < bands; ++band) {
            glBegin(GL_QUADS);
            for (int i = 0; i < 32; ++i) {
                if (plaid) {
                    float middleY =
                        lower->y + (upper->y - lower->y) * (float(band) + .5f) / float(bands);
                    float tone = (i / 2 + int(middleY * 22)) % 2 == 0 ? 1.0f : .48f;
                    glColor3f(color.x * tone, color.y * tone, color.z * tone);
                }
                for (int vertex = 0; vertex < 4; ++vertex) {
                    int edge = vertex == 0 || vertex == 3 ? 1 : 0;
                    float a = float(i + (vertex >= 2 ? 1 : 0)) * 2 * Constants::PI / 32;
                    float u = float(band + edge) / float(bands);
                    Ring r{lower->y + (upper->y - lower->y) * u,
                           lower->width + (upper->width - lower->width) * u,
                           lower->depth + (upper->depth - lower->depth) * u};
                    Vec3 n = normalized({std::sin(a) / r.width,
                                         (lower->width - upper->width) / (upper->y - lower->y),
                                         std::cos(a) / r.depth});
                    glNormal3f(n.x, n.y, n.z);
                    glVertex3f(r.width * std::sin(a), r.y, r.depth * std::cos(a));
                }
            }
            glEnd();
        }
    }
}
void limb(Vec3 from, Vec3 to, float width, float depth, Vec3 color) {
    Vec3 d = to - from;
    glPushMatrix();
    glTranslatef(from.x, from.y, from.z);
    glRotatef(std::acos(std::clamp(-d.y / length(d), -1.0f, 1.0f)) * 180 / Constants::PI, -d.z, 0,
              d.x);
    Draw::ellipsoid({0, -length(d) / 2, 0}, {width, length(d) / 2 + width * .25f, depth}, color, 20,
                    12);
    glPopMatrix();
}
// Solve the knee from hip and ankle positions; the stance foot stays level
// while the other foot lifts and swings forward.
Vec3 kneeBetween(Vec3 hip, Vec3 ankle) {
    constexpr float upper = .59f, lower = .56f;
    Vec3 delta = ankle - hip;
    float d = std::clamp(length(delta), .05f, upper + lower - .001f);
    Vec3 axis = normalized(delta);
    Vec3 bend = normalized(cross({1, 0, 0}, axis)) * -1;
    float along = (upper * upper - lower * lower + d * d) / (2 * d);
    return hip + axis * along + bend * std::sqrt(std::max(0.0f, upper * upper - along * along));
}
} // namespace

Farmer::Farmer() {
    makeRoute(false);
    dayRoute = route;
    makeRoute(true);
    nightRoute = route;
    route = dayRoute;
    pose();
}
void Farmer::initialize(const std::filesystem::path& executable) {
    auto base = std::filesystem::absolute(executable).parent_path() / "assets";
    if (!std::filesystem::exists(base))
        base = "assets";
    portrait = TextureSet::loadBmp(base / "textures/characters/farmer_head.bmp");
    if (portrait) {
        glBindTexture(GL_TEXTURE_2D, portrait);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    } else
        std::cerr << "Farmer portrait unavailable; using the modeled face.\n";
    glDisable(GL_TEXTURE_2D);
}
void Farmer::makeRoute(bool night) {
    route.clear();
    // Derive aisle centers from the same row coordinates that place the wheat.
    const auto& rows = Crop::rowCenters;
    float east = (rows[8] + rows[9]) * .5f, innerEast = (rows[6] + rows[7]) * .5f;
    float west = (rows[3] + rows[4]) * .5f, outerWest = (rows[1] + rows[2]) * .5f;
    std::vector<Vec3> corners = {{0, 0, 0},
                                 {12, 0, 0},
                                 {12, 0, -18},
                                 {9.6f, 0, -21.3f},
                                 {east, 0, -21.3f},
                                 {east, 0, -12},
                                 {east, 0, -3.2f},
                                 {innerEast, 0, -3.2f},
                                 {innerEast, 0, -21.3f},
                                 {0, 0, -21.3f},
                                 {0, 0, -3.2f},
                                 {west, 0, -3.2f},
                                 {west, 0, -12},
                                 {west, 0, -21.3f},
                                 {outerWest, 0, -21.3f},
                                 {outerWest, 0, -3.2f},
                                 {-9, 0, -3.2f},
                                 {-9, 0, 0},
                                 {-18, 0, 0},
                                 {-22, 0, 0},
                                 {-22, 0, 3},
                                 {-18, 0, 6},
                                 {-18, 0, 18},
                                 {-9, 0, 18},
                                 {-9, 0, 6},
                                 {-3, 0, 6},
                                 {-3, 0, 22},
                                 {0, 0, 22},
                                 {0, 0, 15},
                                 {4.5f, 0, 15},
                                 {4.5f, 0, 4.5f},
                                 {14, 0, 4.5f},
                                 {14, 0, 7},
                                 {20.5f, 0, 7},
                                 {20.5f, 0, 16},
                                 {14, 0, 16},
                                 {14, 0, 21.8f},
                                 {4.5f, 0, 21.8f},
                                 {4.5f, 0, 15},
                                 {0, 0, 15},
                                 {0, 0, 2.5f},
                                 {-2.5f, 0, 2.5f},
                                 {-2.5f, 0, 0}};
    if (night)
        corners = {{-14, 0, -1.2f}, {-19, 0, -1.2f}, {-23, 0, -1.2f},  {-23, 0, -8},
                   {-23, 0, -14},   {-17, 0, -14},   {-11.8f, 0, -14}, {-11.8f, 0, -1.2f}};
    auto append = [&](Vec3 p, double wait = 0) {
        if (wait > 0) {
            route.back().rampOut = .55f;
            route.back().time += .275;
        }
        double distance = route.empty() ? 0 : length(p - route.back().point);
        double time = route.empty() ? 0 : route.back().time;
        double total = route.empty() ? 0 : route.back().distance;
        float rampIn = wait == 0 && route.size() > 1 &&
                               route.back().distance == route[route.size() - 2].distance
                           ? .55f
                           : 0.0f;
        if (route.empty() || distance > .00001 || wait > 0)
            route.push_back(
                {p, time + distance / speed + wait + rampIn * .5, total + distance, rampIn, 0});
    };
    for (size_t i = 0; i < corners.size(); ++i) {
        Vec3 p = corners[i], previous = corners[(i + corners.size() - 1) % corners.size()],
             next = corners[(i + 1) % corners.size()];
        if (i == 0) {
            append(p);
            continue;
        }
        if ((!night && (i == 5 || i == 12)) || (night && (i == 3 || i == 5))) {
            append(p);
            append(p, 4.5); // Stop, glance down and reach toward a wheat head.
            continue;
        }
        float radius = std::min({.65f, length(previous - p) * .25f, length(next - p) * .25f});
        Vec3 entry = p + normalized(previous - p) * radius;
        Vec3 exit = p + normalized(next - p) * radius;
        append(entry);
        for (int step = 1; step <= 16; ++step) {
            float u = float(step) / 16;
            append(entry * ((1 - u) * (1 - u)) + p * (2 * u * (1 - u)) + exit * (u * u));
        }
    }
    append(route.front().point);
}
void Farmer::activate(bool night) {
    completedDistance = travelled;
    clock = 0;
    route = night ? nightRoute : dayRoute;
    routine = night ? Routine::Night : Routine::Day;
    entryHeading = heading;
    blendHeading = true;
    pose();
}
void Farmer::transfer(bool toNight) {
    // Retrace the shorter part of the current circuit to its entrance. This
    // preserves the crop aisles and pasture gate instead of cutting diagonally
    // across wheat, walls or fences when the time of day changes.
    std::vector<Vec3> points{location};
    auto next = std::upper_bound(route.begin(), route.end(), clock,
                                 [](double t, const Sample& p) { return t < p.time; });
    double along = travelled - completedDistance;
    if (along < route.back().distance * .5) {
        auto it = next;
        while (it != route.begin()) {
            --it;
            points.push_back(it->point);
        }
    } else {
        for (auto it = next; it != route.end(); ++it)
            points.push_back(it->point);
    }
    if (toNight) {
        points.insert(points.end(), {{-11.8f, 0, 0}, {-11.8f, 0, -1.2f}, nightRoute.front().point});
    } else {
        points.insert(points.end(), {{-11.8f, 0, -1.2f}, {-11.8f, 0, 0}, dayRoute.front().point});
    }
    route.clear();
    route.push_back({location, 0, 0});
    for (Vec3 p : points) {
        double distance = length(p - route.back().point);
        if (distance > .00001)
            route.push_back(
                {p, route.back().time + distance / speed, route.back().distance + distance});
    }
    completedDistance = travelled;
    clock = 0;
    routine = toNight ? Routine::ToBarn : Routine::ToFields;
    entryHeading = heading;
    blendHeading = true;
    pose();
}
void Farmer::update(float dt, bool night) {
    nightRequested = night;
    if (!walkingEnabled || !std::isfinite(dt) || dt <= 0)
        return;
    double remaining = dt;
    while (remaining > .0000001) {
        if ((routine == Routine::Day && night) || (routine == Routine::Night && !night))
            transfer(night);
        bool inTransit = routine == Routine::ToBarn || routine == Routine::ToFields;
        if (inTransit) {
            double slice = std::min(remaining, route.back().time - clock);
            clock += slice;
            remaining -= slice;
            pose();
            if (clock >= route.back().time - .0000001)
                activate(routine == Routine::ToBarn);
        } else {
            double nextTime = clock + remaining;
            if (nextTime >= .8)
                blendHeading = false;
            completedDistance += std::floor(nextTime / route.back().time) * route.back().distance;
            clock = std::fmod(nextTime, route.back().time);
            pose();
            remaining = 0;
        }
    }
}
void Farmer::pose() {
    auto next = std::upper_bound(route.begin(), route.end(), clock,
                                 [](double t, const Sample& p) { return t < p.time; });
    if (next == route.end())
        next = route.end() - 1;
    const auto& a = *(next - 1);
    const auto& b = *next;
    double elapsed = clock - a.time, remaining = b.time - clock;
    float u = float(elapsed / (b.time - a.time));
    Vec3 direction = b.point - a.point;
    bool waiting = length(direction) < .00001f;
    motion = waiting ? 0.0f : 1.0f;
    if (!waiting) {
        double distance = speed * (elapsed - b.rampIn * .5);
        if (b.rampIn > 0 && elapsed < b.rampIn) {
            distance = speed * elapsed * elapsed / (2 * b.rampIn);
            motion = float(elapsed / b.rampIn);
        } else if (b.rampOut > 0 && remaining < b.rampOut) {
            distance = b.distance - a.distance - speed * remaining * remaining / (2 * b.rampOut);
            motion = float(remaining / b.rampOut);
        }
        u = float(distance / (b.distance - a.distance));
    }
    location = a.point + (b.point - a.point) * u;
    travelled = completedDistance + a.distance + (b.distance - a.distance) * u;
    if (waiting)
        direction = a.point - (next - 2)->point;
    // Look ahead along the route to blend the tangent through the rounded corners.
    if (!waiting && b.rampIn == 0 && b.rampOut == 0) {
        double future = std::fmod(clock + .16, route.back().time);
        auto ahead = std::upper_bound(route.begin(), route.end(), future,
                                      [](double t, const Sample& p) { return t < p.time; });
        if (ahead != route.end()) {
            const auto& before = *(ahead - 1);
            float f = float((future - before.time) / (ahead->time - before.time));
            Vec3 look = before.point + (ahead->point - before.point) * f - location;
            if (length(look) > .001f)
                direction = look;
        }
    }
    heading = std::atan2(direction.x, direction.z) * 180 / Constants::PI;
    if (blendHeading && clock < .8)
        heading = entryHeading +
                  std::remainder(heading - entryHeading, 360.0f) * smooth(float(clock / .8));
    else
        blendHeading = false;
    gait = float(std::fmod(travelled / stride, 1.0)) * 2 * Constants::PI;
    inspection = waiting ? smooth(std::min(u, 1 - u) * 6) : 0;
}
const char* Farmer::statusKey() const {
    if (!walkingEnabled)
        return "farmer.stopped";
    if (routine == Routine::ToBarn)
        return "farmer.to_barn";
    if (routine == Routine::ToFields)
        return "farmer.to_fields";
    if (routine == Routine::Night)
        return "farmer.guarding";
    if (inspection > .01f || motion == 0)
        return "farmer.inspecting";
    return location.z < -3 && std::abs(location.x) > 2 && location.x < 9 ? "farmer.crops"
                                                                         : "farmer.walking";
}

void Farmer::head(bool enabled) const {
    Draw::beam({0, 1.88f, 0}, {0, 2.12f, 0}, .105f, skin, 24);
    glPushMatrix();
    glTranslatef(0, 2.18f, 0);
    glRotatef(inspection * -22 + std::sin(float(clock) * .65f) * 4, 0, 1, 0);
    glRotatef(inspection * 16, 1, 0, 0);
    // The online CC0 painted head texture covers the curved face. The rest of
    // the head, hair and straw hat remain geometry visible from every angle.
    Draw::ellipsoid({0, .055f, -.018f}, {.205f, .27f, .19f}, skin, 32, 24);
    for (float side : {-1.0f, 1.0f}) {
        Draw::ellipsoid({side * .207f, .035f, -.003f}, {.043f, .077f, .038f}, skin, 16, 12);
        Draw::ellipsoid({side * .226f, .035f, .014f}, {.015f, .046f, .018f}, {.50f, .29f, .23f}, 12,
                        8);
    }
    Draw::ellipsoid({0, .24f, -.066f}, {.207f, .123f, .177f}, hair, 32, 20);
    Draw::ellipsoid({-.025f, .29f, -.018f}, {.18f, .072f, .14f}, hair, 28, 16);
    const Vec3 straw{.82f, .65f, .33f};
    Draw::ellipsoid({0, .335f, 0}, {.415f, .035f, .35f}, straw, 40, 12);
    Draw::ellipsoid({0, .402f, -.025f}, {.245f, .15f, .225f}, straw, 32, 20);
    tailored({{.355f, .246f, .226f}, {.407f, .245f, .224f}}, leather);
    glColor3f(.58f, .43f, .21f);
    for (float radius : {.29f, .33f, .37f, .40f}) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 64; ++i) {
            float a = float(i) * 2 * Constants::PI / 64;
            glNormal3f(0, 1, 0);
            glVertex3f(radius * std::cos(a), .348f, radius * .84f * std::sin(a));
        }
        glEnd();
    }
    if (enabled && portrait) {
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, portrait);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1, 1, 1);
        // Facial region of TheNess's 150 x 150 head_texture.png (CC0 1.0),
        // converted to BMP without editing. Source and license ship with it.
        auto vertex = [&](float u, float v) {
            float halfWidth = v < .3f ? .065f + .12f * smooth(v / .3f) : .185f;
            float x = u * halfWidth, y = -.207f + v * .53f;
            float nose = .043f * std::exp(-u * u * 34 - (v - .44f) * (v - .44f) * 110);
            float edge = 2 * v - 1;
            float z = .122f + .075f * (1 - u * u) + nose - .07f * edge * edge * edge * edge;
            Vec3 n = normalized({u * .6f, (v - .5f) * .3f, 1});
            glNormal3f(n.x, n.y, n.z);
            glTexCoord2f((75 + x / .185f * 34) / 150, 1 - (148 - v * 76) / 150);
            glVertex3f(x, y, z);
        };
        for (int row = 0; row < 32; ++row) {
            glBegin(GL_QUAD_STRIP);
            for (int col = 0; col <= 32; ++col) {
                float u = -1 + float(col) / 16;
                vertex(u, float(row + 1) / 32);
                vertex(u, float(row) / 32);
            }
            glEnd();
        }
        glPopAttrib();
    } else {
        Draw::ellipsoid({0, -.102f, .108f}, {.158f, .105f, .089f}, hair, 24, 16);
        for (float side : {-1.0f, 1.0f}) {
            Draw::ellipsoid({side * .079f, .068f, .171f}, {.042f, .018f, .016f}, {.88f, .86f, .80f},
                            16, 8);
            Draw::ellipsoid({side * .079f, .068f, .187f}, {.015f, .015f, .008f}, hair);
            Draw::beam({side * .04f, .115f, .17f}, {side * .118f, .12f, .155f}, .013f, hair);
        }
        Draw::ellipsoid({0, .006f, .182f}, {.033f, .063f, .053f}, skin, 16, 12);
        Draw::ellipsoid({0, -.071f, .198f}, {.065f, .018f, .012f}, {.40f, .23f, .19f});
    }
    glPopMatrix();
}

void Farmer::render(bool enabled) const {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(location.x, .055f, location.z);
    glRotatef(heading, 0, 1, 0);
    float bob = motion * .018f * std::cos(gait * 2);
    float hipHeight = 1.17f + bob;
    Draw::ellipsoid({0, 1.085f + bob, 0}, {.222f, .16f, .151f}, trousers, 24, 14);
    // Separate hip, knee and ankle joints; the soles never sink below the path.
    for (float side : {-1.0f, 1.0f}) {
        float phase = std::fmod(gait / (2 * Constants::PI) + (side > 0 ? .5f : 0), 1.0f);
        constexpr float reach = stride * .62f * .5f;
        float z, lift = 0;
        if (phase < .62f)
            z = reach * (1 - 2 * phase / .62f);
        else {
            float swing = (phase - .62f) / .38f;
            z = -reach + 2 * reach * smooth(swing);
            lift = .15f * std::sin(swing * Constants::PI);
        }
        Vec3 hip{side * .133f, hipHeight, 0};
        Vec3 ankle{side * .143f, .135f + lift * motion, z * motion};
        Vec3 knee = kneeBetween(hip, ankle);
        limb(hip, knee, .115f, .125f, trousers);
        Draw::ellipsoid(knee, {.096f, .11f, .105f}, trousers, 20, 12);
        limb(knee, ankle, .088f, .098f, trousers);
        Draw::ellipsoid(ankle + Vec3{0, -.05f, .07f}, {.102f, .083f, .205f}, leather, 24, 14);
        Draw::ellipsoid(ankle + Vec3{0, .045f, -.012f}, {.099f, .15f, .107f}, leather, 20, 12);
        Draw::box(ankle + Vec3{0, -.111f, .055f}, {.197f, .035f, .32f}, {.035f, .032f, .028f});
        for (int lace = 0; lace < 3; ++lace)
            Draw::beam(ankle + Vec3{-.048f, .02f, .04f + float(lace) * .038f},
                       ankle + Vec3{.048f, .02f, .04f + float(lace) * .038f}, .006f,
                       {.20f, .17f, .14f}, 6);
    }
    glPushMatrix();
    glTranslatef(0, bob, 0);
    glRotatef(motion * std::sin(gait) * 1.2f, 0, 0, 1);
    tailored({{1.08f, .23f, .15f},
              {1.25f, .235f, .152f},
              {1.48f, .245f, .17f},
              {1.75f, .29f, .175f},
              {1.88f, .265f, .14f},
              {1.95f, .115f, .10f}},
             shirt, true);
    tailored({{1.085f, .242f, .162f}, {1.3f, .241f, .175f}}, trousers);
    Draw::box({0, 1.485f, .18f}, {.35f, .39f, .033f}, trousers);
    Draw::box({0, 1.49f, .203f}, {.23f, .14f, .022f}, {.19f, .34f, .46f});
    Draw::beam({-.114f, 1.56f, .219f}, {.114f, 1.56f, .219f}, .006f, {.73f, .64f, .43f});
    for (float side : {-1.0f, 1.0f}) {
        Draw::box({side * .164f, 1.763f, .176f}, {.06f, .305f, .026f}, trousers);
        Draw::box({side * .164f, 1.763f, -.157f}, {.06f, .30f, .026f}, trousers);
        Draw::ellipsoid({side * .164f, 1.652f, .199f}, {.022f, .022f, .012f}, {.80f, .63f, .28f},
                        12, 8);
    }
    glColor3f(cuff.x, cuff.y, cuff.z);
    glBegin(GL_TRIANGLES);
    Draw::triangle({-.11f, 1.966f, .085f}, {-.185f, 1.88f, .14f}, {-.062f, 1.80f, .184f});
    Draw::triangle({.11f, 1.966f, .085f}, {.062f, 1.80f, .184f}, {.185f, 1.88f, .14f});
    glEnd();
    for (float side : {-1.0f, 1.0f}) {
        glPushMatrix();
        glTranslatef(side * .288f, 1.79f, 0);
        glRotatef(side * 8, 0, 0, 1);
        bool carrying = side > 0 && (Constants::DEMO_DAYTIME_LIGHTS || nightRequested);
        glRotatef(carrying ? -6
                           : side * std::sin(gait) * 23 * motion -
                                 (side < 0 && !nightRequested ? inspection * 48 : 0),
                  1, 0, 0);
        Draw::ellipsoid({0, -.03f, 0}, {.105f, .11f, .113f}, shirt, 20, 12);
        tailored(
            {{-.39f, .092f, .101f}, {-.28f, .108f, .118f}, {-.12f, .113f, .123f}, {0, .10f, .11f}},
            shirt, true);
        Draw::ellipsoid({0, -.33f, 0}, {.112f, .09f, .115f}, cuff, 20, 12);
        glTranslatef(0, -.39f, 0);
        glRotatef(carrying ? -12
                           : -12 - std::max(0.0f, side * std::sin(gait)) * 15 * motion -
                                 (nightRequested ? 0 : inspection * 20),
                  1, 0, 0);
        limb({0, 0, 0}, {0, -.34f, 0}, .075f, .08f, skin);
        Draw::ellipsoid({0, -.4f, 0}, {.063f, .10f, .04f}, skin, 20, 12);
        for (int finger = 0; finger < 4; ++finger) {
            float x = -.045f + float(finger) * .029f;
            Draw::beam({x, -.445f, 0}, {x, -.515f + .013f * std::abs(float(finger) - 1.5f), .02f},
                       .014f, skin, 10);
        }
        Draw::beam({-side * .05f, -.375f, .007f}, {-side * .08f, -.444f, .026f}, .023f, skin, 12);
        if (carrying) {
            glPushMatrix();
            glTranslatef(0, -.56f, 0);
            const Vec3 metal{.12f, .14f, .13f};
            Draw::beam({-.07f, -.07f, 0}, {0, .08f, 0}, .014f, metal);
            Draw::beam({.07f, -.07f, 0}, {0, .08f, 0}, .014f, metal);
            Draw::box({0, -.10f, 0}, {.22f, .055f, .20f}, metal);
            Draw::box({0, -.37f, 0}, {.22f, .055f, .20f}, metal);
            for (float x : {-.092f, .092f})
                for (float z : {-.082f, .082f})
                    Draw::beam({x, -.11f, z}, {x, -.35f, z}, .013f, metal);
            glPushAttrib(GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            Draw::ellipsoid({0, -.23f, 0}, {.065f, .108f, .06f}, {1, .69f, .18f}, 16, 12);
            glPopAttrib();
            glPopMatrix();
        }
        glPopMatrix();
    }
    head(enabled);
    glPopMatrix();
    glPopMatrix();
    glPopAttrib();
}
void Farmer::light(float amount, bool enabled) const {
    if (!enabled || amount < .001f || (!Constants::DEMO_DAYTIME_LIGHTS && !nightRequested)) {
        glDisable(GL_LIGHT2);
        return;
    }
    Vec3 f = forward(), right{f.z, 0, -f.x};
    Vec3 p = location + right * .44f + f * .2f + Vec3{0, .62f, 0};
    const GLfloat position[]{p.x, p.y, p.z, 1};
    const GLfloat diffuse[]{amount, amount * .70f, amount * .24f, 1};
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_POSITION, position);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, .18f);
    glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, .11f);
}
