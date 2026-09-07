#pragma once
class Windmill {
  public:
    void update(float dt, float windStrength);
    void render() const;
    float rotation() const { return angle; }

  private:
    float angle = 18;
};
