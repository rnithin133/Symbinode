#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class TransformObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    TransformObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float transX = 0.0f, float transY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, int angle = 0, bool clamp = false);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float translateX();
    void setTranslateX(float transX);
    float translateY();
    void setTranslateY(float transY);
    float scaleX();
    void setScaleX(float scaleX);
    float scaleY();
    void setScaleY(float scaleY);
    int rotation();
    void setRotation(int angle);
    bool clampCoords();
    void setClampCoords(bool clamp);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool transformedTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    float m_translateX = 0.0f;
    float m_translateY = 0.0f;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    int m_angle = 0;
    bool m_clamp = false;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
};

class TransformRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    TransformRenderer(QVector2D resolution);
    ~TransformRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void transformateTexture();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_transformedTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int transformFBO = 0;
    QOpenGLShaderProgram *transformShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // TRANSFORM_H