import QtQuick 2.1
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Image {
    id: patternBackground
    source: "/usr/share/powermenu2/images/gear-pattern.png"
    anchors.fill: parent
    fillMode: Image.Tile
    property color color: Theme.highlightColor
    layer.effect: ShaderEffect {
        id: shaderItem
        property color color: patternBackground.color

        fragmentShader: "
            varying mediump vec2 qt_TexCoord0;
            uniform highp float qt_Opacity;
            uniform lowp sampler2D source;
            uniform highp vec4 color;
            void main() {
                highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
            }
        "
    }
    layer.enabled: true
    layer.samplerName: "source"
}
