// BackGround is the background image to be changed.
// ForeGround is the foreground image.
// ForeGroundOpacity is the opacity of the foreground image.
// ForeGroundPosition is the foreground image's location, measured in pixels. It can be negative, and the alignment of the foreground and background's top-left pixels is indicated by (0,0).

function composite(BackGround, ForeGround, ForeGroundOpacity, ForeGroundPosition) {
    var bgData = BackGround.data;
    var fgData = ForeGround.data;
    var width = BackGround.width;
    var height = BackGround.height;
//Please write your code here according to comments

    for (var x = 0; x < ForeGround.width; x++) {    // Loop over each pixel in the foreground image.
        for (var y = 0; y < ForeGround.height; y++) {

            var indexFg = (y * ForeGround.width + x) * 4;                                       // Index for the foreground pixel.
            var indexBg = ((y + ForeGroundPosition.y) * width + x + ForeGroundPosition.x) * 4;  // Index for the background pixel.

            // Foreground RGB and alpha values
            var r1 = fgData[indexFg];
            var g1 = fgData[indexFg + 1];
            var b1 = fgData[indexFg + 2];
            var a1 = fgData[indexFg + 3] * (ForeGroundOpacity / 255); // Adjust opacity

            // Background RGB and alpha values
            var r2 = bgData[indexBg];
            var g2 = bgData[indexBg + 1];
            var b2 = bgData[indexBg + 2];
            var a2 = bgData[indexBg + 3] / 255;

            // Perform alpha blending.
            var a = a1 + (1 - a1) * a2; // Calculate new Alpha value after blending.
            var r = (r1 * a1 + r2 * a2 * (1 - a1)) / a; // Calculate new red value after blending.
            var g = (g1 * a1 + g2 * a2 * (1 - a1)) / a; // Calculate new green value after blending.
            var b = (b1 * a1 + b2 * a2 * (1 - a1)) / a; // Calculate new blue value after blending.

            // Update background pixel with new blended color values.
            bgData[indexBg] = r;
            bgData[indexBg + 1] = g;
            bgData[indexBg + 2] = b;
            bgData[indexBg + 3] = a * 255; // Update alpha value.

        }
    }
}