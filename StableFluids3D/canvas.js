let gCanvas = document.getElementById("sketchCanvas");
{
    let clientW = document.documentElement.clientWidth;
    let clientH = document.documentElement.clientHeight;
    let sideLength = 0.97*((clientW > clientH)? clientH: clientW);
    gCanvas.width = sideLength;
    // gCanvas.width = (clientW >= clientH)? clientW*0.7: sideLength;
    gCanvas.height = sideLength;
}

export default gCanvas;
