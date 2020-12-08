import QtQuick 2.12
import "ColorUtils.js" as ColorUtils
import QtQuick.Controls 2.5

Item {
    property color colorValue: ColorUtils.hsba(hueSlider.value, sbPicker.saturation, sbPicker.brightness)
    property color startingColor
    layer.enabled: true
    layer.samples: 8

    onStartingColorChanged: {
        var r = ColorUtils.getChannelStr(startingColor, 0)
        var g = ColorUtils.getChannelStr(startingColor, 1)
        var b = ColorUtils.getChannelStr(startingColor, 2)
        console.log("rgb " + r + " " + g + " " + b)

        hueSlider.value = ColorUtils.getHsbComponent(r, g, b, 0)/360
        hueSlider.proportionX = (1 - hueSlider.value)

        sbPicker.saturation = ColorUtils.getHsbComponent(r, g, b, 1)
        sbPicker.proportionX = sbPicker.saturation

        sbPicker.brightness = ColorUtils.getHsbComponent(r, g, b, 2)
        sbPicker.proportionY = 1 - sbPicker.brightness
    }

    Picker {
        id: sbPicker
        x: 5
        y: 5
        width: parent.width - 10
        height: parent.height - 65
        hueColor: {
            var v = 1.0-hueSlider.value
            console.debug("v:"+v)

            if(0.0 <= v && v < 0.16) {
                return Qt.rgba(1.0, 0.0, v/0.16, 1.0)
            } else if(0.16 <= v && v < 0.33) {
                return Qt.rgba(1.0 - (v-0.16)/0.17, 0.0, 1.0, 1.0)
            } else if(0.33 <= v && v < 0.5) {
                return Qt.rgba(0.0, ((v-0.33)/0.17), 1.0, 1.0)
            } else if(0.5 <= v && v < 0.76) {
                return Qt.rgba(0.0, 1.0, 1.0 - (v-0.5)/0.26, 1.0)
            } else if(0.76 <= v && v < 0.85) {
                return Qt.rgba((v-0.76)/0.09, 1.0, 0.0, 1.0)
            } else if(0.85 <= v && v <= 1.0) {
                return Qt.rgba(1.0, 1.0 - (v-0.85)/0.15, 0.0, 1.0)
            } else {
                console.log("hue value is outside of expected boundaries of [0, 1]")
                return "red"
                        }
            }

        onSbChanged: {
            sHSB.numberBox.text = Math.round(sbPicker.saturation * 100)
            bHSB.numberBox.text = Math.round(sbPicker.brightness * 100)

            rRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 0)
            gRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 1)
            bRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 2)

            hex.numberBox.text = colorValue
        }
    }

    Item {
        width: parent.width - 35; height: 10
        y: sbPicker.height + 10
        x: 5
        Rectangle {
            width: parent.height
            height: parent.width
            transform: Rotation { origin.x: 0; origin.y: 0; angle: 270}
            y: width
            radius: 4

            gradient: Gradient {
                GradientStop { position: 1.0;  color: "#FF0000" }
                GradientStop { position: 0.85; color: "#FFFF00" }
                GradientStop { position: 0.76; color: "#00FF00" }
                GradientStop { position: 0.5;  color: "#00FFFF" }
                GradientStop { position: 0.33; color: "#0000FF" }
                GradientStop { position: 0.16; color: "#FF00FF" }
                GradientStop { position: 0.0;  color: "#FF0000" }
            }
        }
        ColorSlider { id: hueSlider; width: parent.width - 7; x: 7/2; y: -7*0.5 + 2
        onHueChanged: {
            hHSB.numberBox.text = Math.round(hueValue * 360)

            rRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 0)
            gRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 1)
            bRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 2)

            hex.numberBox.text = colorValue
        }
        }
    }

    Rectangle {
        y: sbPicker.height + 9
        x: parent.width - 20
        width: 12
        height: 12
        radius: 12
        color: colorValue
    }

    Item {
        property bool visibleRGB: false
        id: colorText
        width: parent.width - 40; height: 10
        y: sbPicker.height + 30

        Image {
            x: 5
            y: 5
            source: "qrc:/icons/color-chager.svg"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    colorText.visibleRGB = !colorText.visibleRGB
                }
            }
        }

        NumberBox {
            id: rRGB
            visible: colorText.visibleRGB
            x: 25
            name: "RGB"
            offsetX: 35
            maximumValue: 255
            numberBox.text: ColorUtils.getChannelStr(colorValue, 0)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                gRGB.numberBox.text = gRGB.numberBox.text
                bRGB.numberBox.text = bRGB.numberBox.text

                var hue = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                     bRGB.numberBox.text, 0) / 360
                var saturation = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 1)
                var brightness = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 2)

                hueSlider.value = hue
                hueSlider.proportionX = (1 - hueSlider.value)

                sbPicker.saturation = saturation

                sbPicker.proportionX = sbPicker.saturation

                sbPicker.brightness = brightness
                sbPicker.proportionY = 1 - sbPicker.brightness

                hex.numberBox.text = colorValue
                hHSB.numberBox.text = Math.round(hue * 360)
                sHSB.numberBox.text = Math.round(saturation * 100)
                bHSB.numberBox.text = Math.round(brightness * 100)
            }
        }

        NumberBox {
            id: gRGB
            visible: colorText.visibleRGB
            x: 81
            name: ""
            maximumValue: 255
            numberBox.text: ColorUtils.getChannelStr(colorValue, 1)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                rRGB.numberBox.text = rRGB.numberBox.text
                bRGB.numberBox.text = bRGB.numberBox.text

                var hue = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                     bRGB.numberBox.text, 0) / 360
                var saturation = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 1)
                var brightness = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 2)

                hueSlider.value = hue
                hueSlider.proportionX = (1 - hueSlider.value)

                sbPicker.saturation = saturation

                sbPicker.proportionX = sbPicker.saturation

                sbPicker.brightness = brightness
                sbPicker.proportionY = 1 - sbPicker.brightness

                hex.numberBox.text = colorValue
                hHSB.numberBox.text = Math.round(hue * 360)
                sHSB.numberBox.text = Math.round(saturation * 100)
                bHSB.numberBox.text = Math.round(brightness * 100)
            }
        }

        NumberBox {
            id: bRGB
            visible: colorText.visibleRGB
            x: 116
            name: ""
            maximumValue: 255
            numberBox.text: ColorUtils.getChannelStr(colorValue, 2)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                gRGB.numberBox.text = gRGB.numberBox.text
                rRGB.numberBox.text = rRGB.numberBox.text

                var hue = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                     bRGB.numberBox.text, 0) / 360
                var saturation = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 1)
                var brightness = ColorUtils.getHsbComponent(rRGB.numberBox.text, gRGB.numberBox.text,
                                                            bRGB.numberBox.text, 2)

                hueSlider.value = hue
                hueSlider.proportionX = (1 - hueSlider.value)

                sbPicker.saturation = saturation
                sbPicker.proportionX = sbPicker.saturation

                sbPicker.brightness = brightness
                sbPicker.proportionY = 1 - sbPicker.brightness

                hex.numberBox.text = colorValue
                hHSB.numberBox.text = Math.round(hue * 360)
                sHSB.numberBox.text = Math.round(saturation * 100)
                bHSB.numberBox.text = Math.round(brightness * 100)
            }
        }

        NumberBox {
            id:hHSB
            visible: !colorText.visibleRGB
            x: 25
            name: "HSB"
            offsetX: 35
            maximumValue: 360
            numberBox.text: Math.ceil(hueSlider.value * 360)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                hueSlider.value = numberBox.text / 360
                hueSlider.proportionX = (1 - hueSlider.value)

                hex.numberBox.text = colorValue

                rRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 0)
                gRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 1)
                bRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 2)
            }
        }

        NumberBox {
            id: sHSB
            visible: !colorText.visibleRGB
            x: 81
            name: ""
            maximumValue: 100
            numberBox.text: Math.ceil(sbPicker.saturation * 100)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                sbPicker.saturation = numberBox.text / 100
                sbPicker.proportionX = sbPicker.saturation

                hex.numberBox.text = colorValue

                rRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 0)
                gRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 1)
                bRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 2)
            }
        }

        NumberBox {
            id: bHSB
            visible: !colorText.visibleRGB
            x: 116
            name: ""
            maximumValue: 100
            numberBox.text: Math.ceil(sbPicker.brightness * 100)
            numberBox.onTextEdited: {
                numberBox.text =  numberBox.text === "" ? "0" : numberBox.text
                numberBox.text = numberBox.text.length > 1 && numberBox.text[0] === "0" ?
                            numberBox.text.substring(1, 2) : numberBox.text

                sbPicker.brightness = numberBox.text / 100
                sbPicker.proportionY = 1 - sbPicker.brightness

                hex.numberBox.text = colorValue

                rRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 0)
                gRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 1)
                bRGB.numberBox.text = ColorUtils.getChannelStr(colorValue, 2)
            }
        }

        NumberBox {
            id: hex
            x: 166
            name: "HEX"
            offsetX: 35
            boxWidth: 50
            numberBox.text: colorValue
            numberBox.validator:  RegExpValidator{
                            regExp: /#?[0-9a-fA-F]*/
                        }
            numberBox.onTextEdited: {
                if(numberBox.text[0] !== '#')
                    numberBox.text = '#' + numberBox.text;

                numberBox.text = numberBox.text.length < 8 ? numberBox.text : numberBox.text.substring(0, 7)
            }

            numberBox.onEditingFinished: {
                var r = ColorUtils.getChannelStr(numberBox.text, 0)
                var g = ColorUtils.getChannelStr(numberBox.text, 1)
                var b = ColorUtils.getChannelStr(numberBox.text, 2)

                rRGB.numberBox.text = r
                gRGB.numberBox.text = g
                bRGB.numberBox.text = b

                hueSlider.value = ColorUtils.getHsbComponent(r, g, b, 0)/360
                hueSlider.proportionX = (1 - hueSlider.value)

                sbPicker.saturation = ColorUtils.getHsbComponent(r, g, b, 1)

                sbPicker.proportionX = sbPicker.saturation

                sbPicker.brightness = ColorUtils.getHsbComponent(r, g, b, 2)
                console.log(sbPicker.brightness)
                sbPicker.proportionY = 1 - sbPicker.brightness

                hHSB.numberBox.text = Math.round(hueSlider.value * 360)
                sHSB.numberBox.text = Math.round(sbPicker.saturation * 100)
                bHSB.numberBox.text = Math.round(sbPicker.brightness * 100)
            }
        }
    }
}