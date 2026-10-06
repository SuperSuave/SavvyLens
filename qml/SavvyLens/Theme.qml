pragma Singleton
import QtQuick 2.15

// Shared design tokens for SavvyLens QML views.
// Colors follow the dark palette used by the widget stylesheet in src/app/main.cpp.
QtObject {
    // Surfaces
    readonly property color background: "#1e1e1e"
    readonly property color surface: "#2d2d2d"
    readonly property color surfaceRaised: "#353535"
    readonly property color surfaceInset: "#252525"
    readonly property color popoverSurface: "#383838"
    readonly property color chrome: "#262626"
    readonly property color chromeRaised: "#303030"
    readonly property color transparent: "transparent"

    // Text
    readonly property color textPrimary: "#dcdcdc"
    readonly property color textMuted: "#a8a8a8"
    readonly property color textFaint: "#7a7a7a"
    readonly property color textDisabled: "#5f5f5f"
    readonly property color textInverse: "#1e1e1e"

    // Borders
    readonly property color border: "#4a4a4a"
    readonly property color borderStrong: "#5f5f5f"
    readonly property color divider: "#444444"
    readonly property color tooltipBorder: "#5a5a5a"

    // Interaction
    readonly property color accent: "#409cff"
    readonly property color accentMuted: "#2f6db3"
    readonly property color accentSubtle: "#1f3550"
    readonly property color hover: "#404040"
    readonly property color pressed: "#4a4a4a"
    readonly property color focus: "#409cff"
    readonly property color disabled: "#3a3a3a"
    readonly property color selection: "#24476e"
    readonly property color selectionBorder: "#409cff"

    // Status
    readonly property color success: "#4caf50"
    readonly property color successSubtle: "#1f3a21"
    readonly property color successBorder: "#2e6b31"
    readonly property color successStrongBorder: "#4caf50"
    readonly property color warning: "#e0a030"
    readonly property color warningSubtle: "#3d3020"
    readonly property color warningBorder: "#7a5a20"
    readonly property color errorSubtle: "#4a2222"
    readonly property color errorBorder: "#a03a3a"
    readonly property color destructiveSubtle: "#4a2222"
    readonly property color neutral: "#8a8a8a"
    readonly property color neutralSubtle: "#333333"
    readonly property color neutralBorder: "#555555"
    readonly property color readOnly: "#9a8cd0"
    readonly property color readOnlySubtle: "#2c2840"
    readonly property color readOnlyBorder: "#5a4f8a"

    // Spacing
    readonly property int spacingXSmall: 2
    readonly property int spacingSmall: 4
    readonly property int spacingMedium: 8
    readonly property int spacingLarge: 12

    // Radii
    readonly property int radiusSmall: 3
    readonly property int radiusMedium: 6
    readonly property int radiusLarge: 10
    readonly property int radiusPill: 999

    readonly property int topBarHeight: 36

    // Studio workspace
    readonly property real studioScale: 1.0
    readonly property int studioSpacingSmall: Math.round(4 * studioScale)
    readonly property int studioSpacingMedium: Math.round(8 * studioScale)
    readonly property int studioSpacingLarge: Math.round(16 * studioScale)
    readonly property int studioSpacingXLarge: Math.round(24 * studioScale)
    readonly property int studioRadiusMedium: Math.round(6 * studioScale)
    readonly property int studioRadiusLarge: Math.round(10 * studioScale)
    readonly property int studioTextSmall: Math.round(11 * studioScale)
    readonly property int studioTextBody: Math.round(13 * studioScale)
    readonly property int studioTextSection: Math.round(15 * studioScale)
    readonly property int studioTextTitle: Math.round(18 * studioScale)
    readonly property int studioTextPageTitle: Math.round(22 * studioScale)
    readonly property int studioTopBarHeight: Math.round(44 * studioScale)
    readonly property int studioTabBarHeight: Math.round(32 * studioScale)
    readonly property int studioNavigationRailWidth: Math.round(64 * studioScale)
    readonly property int studioInspectorWidth: Math.round(300 * studioScale)
}
