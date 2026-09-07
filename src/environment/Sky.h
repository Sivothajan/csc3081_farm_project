#pragma once
class Sky {
  public:
    bool night = false;
    float nightAmount = 0;
    void update(float dt);
    void background() const;
    void render(float time) const;
};
