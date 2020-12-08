#include "mix.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

MixObject::MixObject(QQuickItem *parent, QVector2D resolution, float factor, int mode):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_factor(factor), m_mode(mode)
{
     setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *MixObject::createRenderer() const {
    return new MixRenderer(m_resolution);
}

unsigned int MixObject::maskTexture() {
    return  m_maskTexture;
}

void MixObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
}

unsigned int MixObject::firstTexture() {
    return m_firstTexture;
}

void MixObject::setFirstTexture(unsigned int texture) {
    m_firstTexture = texture;
}

unsigned int MixObject::secondTexture() {
    return m_secondTexture;
}

void MixObject::setSecondTexture(unsigned int texture) {
    m_secondTexture = texture;
}

QVariant MixObject::factor() {
    return m_factor;
}

void MixObject::setFactor(QVariant f) {
    m_factor = f;
}

int MixObject::mode() {
    return m_mode;
}

void MixObject::setMode(int mode) {
    m_mode = mode;
}

QVector2D MixObject::resolution() {
    return m_resolution;
}

void MixObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

unsigned int &MixObject::texture() {
    return m_texture;
}

void MixObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

MixRenderer::~MixRenderer() {
    delete mixShader;
    delete renderTexture;
}

MixRenderer::MixRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();

    mixShader = new QOpenGLShaderProgram();
    mixShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    mixShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mix.frag");
    mixShader->link();

    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();

    mixShader->bind();
    mixShader->setUniformValue(mixShader->uniformLocation("firstTexture"), 0);
    mixShader->setUniformValue(mixShader->uniformLocation("secondTexture"), 1);
    mixShader->setUniformValue(mixShader->uniformLocation("factorTexture"), 2);
    mixShader->setUniformValue(mixShader->uniformLocation("maskTexture"), 3);
    mixShader->release();

    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenFramebuffers(1, &mixFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mixFBO);
    glGenTextures(1, &mixTexture);
    glBindTexture(GL_TEXTURE_2D, mixTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mixTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLFramebufferObject *MixRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void MixRenderer::synchronize(QQuickFramebufferObject *item) {
    MixObject *mixItem = static_cast<MixObject*>(item);
    if(mixItem->resUpdated) {
        mixItem->resUpdated = false;
        m_resolution = mixItem->resolution();
        updateTextureRes();
    }
    if(mixItem->mixedTex) {
        mixItem->mixedTex = false;
        firstTexture = mixItem->firstTexture();
        secondTexture = mixItem->secondTexture();
        if(firstTexture || secondTexture) {
            maskTexture = mixItem->maskTexture();
            mixShader->bind();
            mixShader->setUniformValue(mixShader->uniformLocation("useFactorTex"), mixItem->useFactorTexture);
            mixShader->setUniformValue(mixShader->uniformLocation("mode"), mixItem->mode());
            mixShader->setUniformValue(mixShader->uniformLocation("useMask"), maskTexture);
            mixShader->release();
            if(mixItem->useFactorTexture) {
                factorTexture = mixItem->factor().toUInt();
            }
            else {
                mixFactor = mixItem->factor().toFloat();
            }
            mix();
            mixItem->setTexture(mixTexture);
            if(mixItem->selectedItem) {
               mixItem->updatePreview(mixTexture, true);
            }
        }
    }    
}

void MixRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(mixTexture) {
        renderTexture->bind();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mixTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        renderTexture->release();
    }
}

void MixRenderer::mix() {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, mixFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    mixShader->bind();
    mixShader->setUniformValue(mixShader->uniformLocation("mixFactor"), mixFactor);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, firstTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, secondTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, factorTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    mixShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MixRenderer::updateTextureRes() {
    glBindTexture(GL_TEXTURE_2D, mixTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}