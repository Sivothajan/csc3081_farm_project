#include <GL/freeglut.h>
#include "core/Scene.h"
#include "core/Capture.h"
#include "core/Verification.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <string>

namespace {
Scene scene;
std::array<bool, 256> keys{}, special{};
int width = 1280, height = 800, previousTime = 0, frame = 0;
int benchmarkFrames = 0, benchmarkStart = 0;
bool dragging = false;
bool closing = false;
int mouseX = 0, mouseY = 0;
std::string capturePath;
void windowClosed() {
    // Global GLUT timers can still fire in the event-loop iteration that destroys
    // the window. The close callback runs before its OpenGL context disappears.
    closing = true;
}
void requestExit() {
    closing = true;
    glutLeaveMainLoop();
}
void display() {
    if (closing || glutGetWindow() == 0)
        return;
    scene.background();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    scene.camera.apply();
    scene.render(width, height);
    ++frame;
    if (!capturePath.empty() && frame == 3) {
        glFinish();
        bool ok = captureFrame(capturePath, width, height);
        GLenum error = glGetError();
        std::cout << "Capture: " << capturePath << " saved=" << ok << " GL error=" << error << '\n';
        if (!ok || error != GL_NO_ERROR)
            std::exit(1);
        requestExit();
        return;
    }
    glutSwapBuffers();
    if (benchmarkFrames > 0) {
        if (glGetError() != GL_NO_ERROR) {
            std::cerr << "OpenGL error during benchmark\n";
            std::exit(1);
        }
        if (frame >= benchmarkFrames) {
            float seconds = float(glutGet(GLUT_ELAPSED_TIME) - benchmarkStart) / 1000.0f;
            std::cout << "Benchmark: " << frame << " frames in " << seconds << " seconds, "
                      << float(frame) / seconds << " FPS\n";
            requestExit();
        }
    }
}
void reshape(int w, int h) {
    width = std::max(w, 1);
    height = std::max(h, 1);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, double(width) / height, 0.08, 160);
    glMatrixMode(GL_MODELVIEW);
}
void update(int) {
    // Timers belong to the application, not the window. Ignore an outstanding
    // callback after title-bar closure, and do not schedule another timer.
    if (closing || glutGetWindow() == 0)
        return;
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = std::clamp(float(now - previousTime) / 1000.0f, 0.0f, 0.05f);
    previousTime = now;
    scene.camera.update(dt, keys, special);
    // Fixed-time screenshots use the real renderer without timer-dependent changes.
    if (capturePath.empty())
        scene.update(dt);
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
void keyboard(unsigned char key, int, int) {
    unsigned char lower = static_cast<unsigned char>(std::tolower(key));
    if (!keys[lower])
        scene.key(lower);
    keys[lower] = true;
    if (key == 27)
        requestExit();
}
void keyboardUp(unsigned char key, int, int) {
    keys[static_cast<unsigned char>(std::tolower(key))] = false;
}
void specialDown(int key, int, int) {
    if (key >= 0 && key < 256) {
        if (!special[key])
            scene.specialKey(key);
        special[key] = true;
    }
}
void specialUp(int key, int, int) {
    if (key >= 0 && key < 256)
        special[key] = false;
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON)
        dragging = state == GLUT_DOWN;
    mouseX = x;
    mouseY = y;
}
void motion(int x, int y) {
    if (dragging)
        scene.camera.look(float(x - mouseX) * 0.2f, float(mouseY - y) * 0.2f);
    mouseX = x;
    mouseY = y;
}
} // namespace
int main(int argc, char** argv) {
    RunOptions options;
    try {
        options = RunOptions::parse(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    capturePath = options.capture;
    width = options.width;
    height = options.height;
    benchmarkFrames = options.benchmarkFrames;
    // Keep application switches out of GLUT's command line parser.
    int glutArgc = 1;
    glutInit(&glutArgc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("CSC3081 | Interactive 3D Farm");
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCloseFunc(windowClosed);
    glutIgnoreKeyRepeat(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    scene.initialize(argv[0]);
    if (options.selfTest) {
        bool passed = Verification::run(scene);
        glutDestroyWindow(glutGetWindow());
        return passed ? 0 : 1;
    }
    Verification::prepare(scene, options);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    previousTime = glutGet(GLUT_ELAPSED_TIME);
    benchmarkStart = previousTime;
    glutTimerFunc(0, update, 0);
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\nRenderer: " << glGetString(GL_RENDERER)
              << '\n';
    glutMainLoop();
    return 0;
}
