#include "tilenode.h"
#include <iostream>

TileNode::TileNode(QQuickItem *parent, QVector2D resolution, float offsetX, float offsetY, int columns,
                   int rows, float scaleX, float scaleY, int rotation, float randPosition,
                   float randRotation, float randScale, float maskStrength, int inputsCount,
                   bool keepProportion): Node(parent, resolution), m_offsetX(offsetX), m_offsetY(offsetY),
    m_columns(columns), m_rows(rows), m_scaleX(scaleX), m_scaleY(scaleY), m_rotationAngle(rotation),
    m_randPosition(randPosition), m_randRotation(randRotation), m_randScale(randScale),
    m_maskStrength(maskStrength), m_inputsCount(inputsCount), m_keepProportion(keepProportion)
{
    preview = new TileObject(grNode, m_resolution, m_offsetX, m_offsetY, m_columns, m_rows, m_scaleX, m_scaleY, m_rotationAngle, m_randPosition, m_randRotation, m_randScale, m_maskStrength, m_inputsCount, m_keepProportion);
    float s = scaleView();
    setHeight(435*s);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &TileNode::updateScale);
    connect(this, &TileNode::changeSelected, this, &TileNode::updatePrev);
    connect(preview, &TileObject::changedTexture, this, &TileNode::setOutput);
    connect(preview, &TileObject::updatePreview, this, &TileNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &TileObject::setResolution);
    connect(this, &TileNode::offsetXChanged, preview, &TileObject::setOffsetX);
    connect(this, &TileNode::offsetYChanged, preview, &TileObject::setOffsetY);
    connect(this, &TileNode::columnsChanged, preview, &TileObject::setColumns);
    connect(this, &TileNode::rowsChanged, preview, &TileObject::setRows);
    connect(this, &TileNode::scaleXChanged, preview, &TileObject::setScaleX);
    connect(this, &TileNode::scaleYChanged, preview, &TileObject::setScaleY);
    connect(this, &TileNode::rotationAngleChanged, preview, &TileObject::setRotationAngle);
    connect(this, &TileNode::randPositionChanged, preview, &TileObject::setRandPosition);
    connect(this, &TileNode::randRotationChanged, preview, &TileObject::setRandRotation);
    connect(this, &TileNode::randScaleChanged, preview, &TileObject::setRandScale);
    connect(this, &TileNode::maskStrengthChanged, preview, &TileObject::setMaskStrength);
    connect(this, &TileNode::inputsCountChanged, preview, &TileObject::setInputsCount);
    connect(this, &TileNode::keepProportionChanged, preview, &TileObject::setKeepProportion);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/TileProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startOffsetX", m_offsetX);
    propertiesPanel->setProperty("startOffsetY", m_offsetY);
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_rotationAngle);
    propertiesPanel->setProperty("startRandPosition", m_randPosition);
    propertiesPanel->setProperty("startRandRotation", m_randRotation);
    propertiesPanel->setProperty("startRandScale", m_randScale);
    propertiesPanel->setProperty("startMask", m_maskStrength);
    propertiesPanel->setProperty("startInputsCount", m_inputsCount);
    propertiesPanel->setProperty("startKeepProportion", m_keepProportion);
    connect(propertiesPanel, SIGNAL(offsetXChanged(qreal)), this, SLOT(updateOffsetX(qreal)));
    connect(propertiesPanel, SIGNAL(offsetYChanged(qreal)), this, SLOT(updateOffsetY(qreal)));
    connect(propertiesPanel, SIGNAL(columnsChanged(int)), this, SLOT(updateColums(int)));
    connect(propertiesPanel, SIGNAL(rowsChanged(int)), this, SLOT(updateRows(int)));
    connect(propertiesPanel, SIGNAL(scaleXChanged(qreal)), this, SLOT(updateScaleX(qreal)));
    connect(propertiesPanel, SIGNAL(scaleYChanged(qreal)), this, SLOT(updateScaleY(qreal)));
    connect(propertiesPanel, SIGNAL(rotationAngleChanged(int)), this, SLOT(updateRotationAngle(int)));
    connect(propertiesPanel, SIGNAL(randPositionChanged(qreal)), this, SLOT(updateRandPosition(qreal)));
    connect(propertiesPanel, SIGNAL(randRotationChanged(qreal)), this, SLOT(updateRandRotation(qreal)));
    connect(propertiesPanel, SIGNAL(randScaleChanged(qreal)), this, SLOT(updateRandScale(qreal)));
    connect(propertiesPanel, SIGNAL(maskChanged(qreal)), this, SLOT(updateMaskStrength(qreal)));
    connect(propertiesPanel, SIGNAL(inputsCountChanged(int)), this, SLOT(updateInputsCount(int)));
    connect(propertiesPanel, SIGNAL(keepProportionChanged(bool)), this, SLOT(updateKeepProportion(bool)));
    createSockets(2, 1);
    createAdditionalInputs(5);
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < m_inputsCount - 1) {
            s->setVisible(true);
        }
        else {
            s->setVisible(false);
        }
    }
    setTitle("Tile");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    m_additionalInputs[0]->setTip("Texture");
    m_additionalInputs[1]->setTip("Texture");
    m_additionalInputs[2]->setTip("Texture");
    m_additionalInputs[3]->setTip("Texture");
    m_additionalInputs[4]->setTip("Texture");
}

TileNode::~TileNode() {
    delete preview;
}

void TileNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    preview->setTile1(m_additionalInputs[0]->value().toUInt());
    preview->setTile2(m_additionalInputs[1]->value().toUInt());
    preview->setTile3(m_additionalInputs[2]->value().toUInt());
    preview->setTile4(m_additionalInputs[3]->value().toUInt());
    preview->setTile5(m_additionalInputs[4]->value().toUInt());
}

void TileNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 17;
    json["offsetX"] = m_offsetX;
    json["offsetY"] = m_offsetY;
    json["columns"] = m_columns;
    json["rows"] = m_rows;
    json["scaleX"] = m_scaleX;
    json["scaleY"] = m_scaleY;
    json["rotation"] = m_rotationAngle;
    json["randPosition"] = m_randPosition;
    json["randRotation"] = m_randRotation;
    json["randScale"] = m_randScale;
    json["maskStrength"] = m_maskStrength;
    json["inputsCount"] = m_inputsCount;
    json["keepProportion"] = m_keepProportion;
}

void TileNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("offsetX")) {
        m_offsetX = json["offsetX"].toVariant().toFloat();
    }
    if(json.contains("offsetY")) {
        m_offsetY = json["offsetY"].toVariant().toFloat();
    }
    if(json.contains("columns")) {
        m_columns = json["columns"].toVariant().toInt();
    }
    if(json.contains("rows")) {
        m_rows = json["rows"].toVariant().toInt();
    }
    if(json.contains("scaleX")) {
        m_scaleX = json["scaleX"].toVariant().toFloat();
    }
    if(json.contains("scaleY")) {
        m_scaleY = json["scaleY"].toVariant().toFloat();
    }
    if(json.contains("rotation")) {
        m_rotationAngle = json["rotation"].toVariant().toFloat();
    }
    if(json.contains("randPosition")) {
        m_randPosition = json["randPosition"].toVariant().toFloat();
    }
    if(json.contains("randRotation")) {
        m_randRotation = json["randRotation"].toVariant().toFloat();
    }
    if(json.contains("randScale")) {
        m_randScale = json["randScale"].toVariant().toFloat();
    }
    if(json.contains("maskStrength")) {
        m_maskStrength = json["maskStrength"].toVariant().toFloat();
    }
    if(json.contains("inputsCount")) {
        m_inputsCount = json["inputsCount"].toInt();
    }
    if(json.contains("keepProportion")) {
        m_keepProportion = json["keepProportion"].toBool();
    }
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < m_inputsCount - 1) {
            s->setVisible(true);
        }
        else {
            s->setVisible(false);
        }
    }
    propertiesPanel->setProperty("startOffsetX", m_offsetX);
    propertiesPanel->setProperty("startOffsetY", m_offsetY);
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_rotationAngle);
    propertiesPanel->setProperty("startRandPosition", m_randPosition);
    propertiesPanel->setProperty("startRandRotation", m_randRotation);
    propertiesPanel->setProperty("startRandScale", m_randScale);
    propertiesPanel->setProperty("startMask", m_maskStrength);
    propertiesPanel->setProperty("startInputsCount", m_inputsCount);
    propertiesPanel->setProperty("startKeepProportion", m_keepProportion);
}

float TileNode::offsetX() {
    return m_offsetX;
}

void TileNode::setOffsetX(float offset) {
    m_offsetX = offset;
    offsetXChanged(offset);
}

float TileNode::offsetY() {
    return m_offsetY;
}

void TileNode::setOffsetY(float offset) {
    m_offsetY = offset;
    offsetYChanged(offset);
}

int TileNode::columns() {
    return m_columns;
}

void TileNode::setColumns(int columns) {
    m_columns = columns;
    columnsChanged(columns);
}

int TileNode::rows() {
    return m_rows;
}

void TileNode::setRows(int rows) {
    m_rows = rows;
    rowsChanged(rows);
}

float TileNode::scaleX() {
    return m_scaleX;
}

void TileNode::setScaleX(float scale) {
    m_scaleX = scale;
    scaleXChanged(scale);
}

float TileNode::scaleY() {
    return m_scaleY;
}

void TileNode::setScaleY(float scale) {
    m_scaleY = scale;
    scaleYChanged(scale);
}

int TileNode::rotationAngle() {
    return m_rotationAngle;
}

void TileNode::setRotationAngle(int angle) {
    m_rotationAngle = angle;
    rotationAngleChanged(angle);
}

float TileNode::randPosition() {
    return m_randPosition;
}

void TileNode::setRandPosition(float rand) {
    m_randPosition = rand;
    randPositionChanged(rand);
}

float TileNode::randRotation() {
    return m_randRotation;
}

void TileNode::setRandRotation(float rand) {
    m_randRotation = rand;
    randRotationChanged(rand);
}

float TileNode::randScale() {
    return m_randScale;
}

void TileNode::setRandScale(float rand) {
    m_randScale = rand;
    randScaleChanged(rand);
}

float TileNode::maskStrength() {
    return m_maskStrength;
}

void TileNode::setMaskStrength(float mask) {
    m_maskStrength = mask;
    maskStrengthChanged(mask);
}

int TileNode::inputsCount() {
    return m_inputsCount;
}

void TileNode::setInputsCount(int count) {
    m_inputsCount = count;
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < count - 1) {
            s->setVisible(true);
            for(auto edge: s->getEdges()) {
                edge->setVisible(true);
            }
        }
        else {
            s->setVisible(false);
            for(auto edge: s->getEdges()) {
                edge->setVisible(false);
            }
        }
    }
    inputsCountChanged(count);
}

bool TileNode::keepProportion() {
    return m_keepProportion;
}

void TileNode::setKeepProportion(bool keep) {
    m_keepProportion = keep;
    keepProportionChanged(keep);
}

void TileNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void TileNode::updateScale(float scale) {
    setHeight(435*scale);
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void TileNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void TileNode::previewGenerated() {
    preview->tiledTex = true;
    preview->update();
}

void TileNode::updateOffsetX(qreal offset) {
    setOffsetX(offset);
    operation();
    dataChanged();
}

void TileNode::updateOffsetY(qreal offset) {
    setOffsetY(offset);
    operation();
    dataChanged();
}

void TileNode::updateColums(int columns) {
    setColumns(columns);
    operation();
    dataChanged();
}

void TileNode::updateRows(int rows) {
    setRows(rows);
    operation();
    dataChanged();
}

void TileNode::updateScaleX(qreal scale) {
    setScaleX(scale);
    operation();
    dataChanged();
}

void TileNode::updateScaleY(qreal scale) {
    setScaleY(scale);
    operation();
    dataChanged();
}

void TileNode::updateRotationAngle(int angle) {
    setRotationAngle(angle);
    operation();
    dataChanged();
}

void TileNode::updateRandPosition(qreal rand) {
    setRandPosition(rand);
    operation();
    dataChanged();
}

void TileNode::updateRandRotation(qreal rand) {
    setRandRotation(rand);
    operation();
    dataChanged();
}

void TileNode::updateRandScale(qreal rand) {
    setRandScale(rand);
    operation();
    dataChanged();
}

void TileNode::updateMaskStrength(qreal mask) {
    setMaskStrength(mask);
    operation();
    dataChanged();
}

void TileNode::updateInputsCount(int count) {
    setInputsCount(count);
    operation();
    dataChanged();
}

void TileNode::updateKeepProportion(bool keep) {
    setKeepProportion(keep);
    operation();
    dataChanged();
}