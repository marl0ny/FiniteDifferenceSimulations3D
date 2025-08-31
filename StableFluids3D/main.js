import gCanvas from "./canvas.js";
import { gl, gMainRenderWindow, TextureParams, RenderTarget, 
         Quad, Vec2, IVec2, IVec3, Vec3, Vec4, Complex,
         Quaternion, mul, add, div,
         withConfig,
         get3DFrom2DTextureCoordinates,
         sub,
         MultidimensionalDataQuad,
         get2DFrom3DDimensions} from "./gl-wrappers.js";
import { getShader } from "./shaders.js";
import { VolumeRender } from "./volume-render.js";
import { PlanarSlices } from "./planar-slice.js";

class SimulationParameters {
    t;
    dt;
    addForces;
    dimensions;
    gridDimensions;
    gridDimensions2D;
    velocityDiffusionSteps;
    velocityDiffusionStrength;
    materialDiffusionSteps;
    materialDiffusionStrength;
    pressureSteps;
    useMacCormackScheme;
    constructor(dt, diffusionStrength,
                dimensions, gridDimensions) {
        this.t = 0.0;
        this.dt = dt;
        this.addForces = false;
        this.dimensions = dimensions;
        this.gridDimensions = gridDimensions;
        this.gridDimensions2D = get2DFrom3DDimensions(this.gridDimensions);
        this.useMacCormackScheme = true;
        this.velocityDiffusionSteps = 6;
        this.velocityDiffusionStrength = diffusionStrength;
        this.materialDiffusionSteps = 3;
        this.materialDiffusionStrength = 0.00004;
        this.pressureSteps = 20;
    }
}

let gSimParams = new SimulationParameters(
    0.5, 0.001,
    // new Vec3(64.0, 64.0, 64.0),
    // new IVec3(64, 64, 64)
    new Vec3(8.0, 8.0, 8.0),
    new IVec3(128, 128, 128)
    // new Vec3(256.0, 256.0, 256.0),
    // new IVec3(256, 256, 256)
);

class MainGLSLPrograms {
    constructor() {
        this.copy
            = Quad.makeProgramFromSource(
                getShader('./shaders/util/copy.frag')
            );
        this.zero
            = Quad.makeProgramFromSource(
                getShader('./shaders/util/zero.frag')
            );
        this.scale 
            = Quad.makeProgramFromSource(
                getShader('./shaders/util/scale.frag')
            );
        this.initDist = Quad.makeProgramFromSource(
            getShader('./shaders/sketch/init-dist.frag')
        );
        this.advection = Quad.makeProgramFromSource(
            getShader('./shaders/advection.frag')
        );
        this.advectionHigherOrder = Quad.makeProgramFromSource(
            getShader('./shaders/advection-higher-or.frag')
        );
        this.divergence = Quad.makeProgramFromSource(
            getShader('./shaders/divergence.frag')
        );
        this.diffusionIter = Quad.makeProgramFromSource(
            getShader('./shaders/diffusion-iter.frag')
        );
        this.addForces = Quad.makeProgramFromSource(
            getShader('./shaders/add-forces.frag')
        );
        this.addToDensity = Quad.makeProgramFromSource(
            getShader('./shaders/sketch/add-to-density.frag')
        );
        this.pressureIter = Quad.makeProgramFromSource(
            getShader('./shaders/pressure-iter.frag')
        );
        this.subtractGradP = Quad.makeProgramFromSource(
            getShader('./shaders/subtract-gradp.frag')
        );
        this.densityView = Quad.makeProgramFromSource(
            getShader('./shaders/density-view.frag')
        );
    }
}

const GLSL_PROGRAMS = new MainGLSLPrograms();

const TEX_PARAMS_HALF_FLOAT = new TextureParams(
    (gl.version === 1)? gl.RGBA16F: gl.R16F,
    gSimParams.gridDimensions2D.ind[0],
    gSimParams.gridDimensions2D.ind[1], true,
    gl.REPEAT, gl.REPEAT, gl.LINEAR, gl.LINEAR
);

const TEX_PARAMS_FLOAT = new TextureParams(
    (gl.version === 1)? gl.RGBA32F: gl.R32F,
    gSimParams.gridDimensions2D.ind[0],
    gSimParams.gridDimensions2D.ind[1], true,
    gl.REPEAT, gl.REPEAT, gl.LINEAR, gl.LINEAR
);

const TEX_PARAMS_HALF_FLOAT4 = new TextureParams(
    gl.RGBA16F,
    gSimParams.gridDimensions2D.ind[0],
    gSimParams.gridDimensions2D.ind[1], true,
    gl.REPEAT, gl.REPEAT, gl.LINEAR, gl.LINEAR
);

const TEX_PARAMS_FLOAT4 = new TextureParams(
    gl.RGBA32F,
    gSimParams.gridDimensions2D.ind[0],
    gSimParams.gridDimensions2D.ind[1], true,
    gl.REPEAT, gl.REPEAT, gl.LINEAR, gl.LINEAR
);

class Frames {
    constructor() {
        this.target = gMainRenderWindow;
        this.velocity1 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.velocity2 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.advForward = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.advReverse = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.matter1 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.matter2 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.extra = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.divergenceVelocity
            = new MultidimensionalDataQuad(
                [...gSimParams.gridDimensions.ind], TEX_PARAMS_FLOAT);
        this.forces = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4);
        this.diffusionIter1 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4
        );
        this.diffusionIter2 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_HALF_FLOAT4
        );
        this.pressureIter1 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_FLOAT
        );
        this.pressureIter2 = new MultidimensionalDataQuad(
            [...gSimParams.gridDimensions.ind], TEX_PARAMS_FLOAT
        );
    }
}

let gFrames = new Frames();

let gViewMode = document.getElementById("viewMode").value;

const VIEW_MODE = {
    PLANAR_SLICES: 1,
    VOLUME_RENDER: 2
};

document.getElementById("viewMode").addEventListener(
    "change", e => {
        gViewMode = e.target.value;
        // console.log(`view mode switched ${e.target.value}`);
    }
);

function initialStep() {
    /* addBlob(
        new Vec3(0.25, 0.25, 0.25),
        mul(0.1*gSimParams.dimensions.ind[0]
             / gSimParams.gridDimensions.ind[0],
        new Vec3(1.0, 1.0, 1.0)),
        new Vec3(1.0, 0.0, 0.0),
        new Vec3(0.07, 0.07, 0.07)
    );
    addBlob(
        new Vec3(0.75, 0.75, 0.75),
        mul(-0.1*gSimParams.dimensions.ind[0]
             / gSimParams.gridDimensions.ind[0],
            new Vec3(1.0, 1.0, 1.0)),
        new Vec3(0.0, 0.0, 1.0),
        new Vec3(0.07, 0.07, 0.07)
    );
    gFrames.pressureIter1.draw(
        GLSL_PROGRAMS.copy,
        {tex: gFrames.matter1}
    );
    gFrames.pressureIter2.draw(
        GLSL_PROGRAMS.copy,
        {tex: gFrames.matter2}
    );*/
}
initialStep();

function addBlob(position, force, color, sigma) {
    gFrames.extra.draw(
        GLSL_PROGRAMS.addToDensity,
        {
            sigma: new Vec3(
                sigma.x, sigma.y, sigma.z
            ),
            location: new Vec3(
                position.x, position.y, position.z),
            amplitude: new Vec4(
                color.x, color.y, color.z, 0.0),
            tex: gFrames.matter1,
            texelDimensions3D: gSimParams.gridDimensions,
            texelDimensions2D: gSimParams.gridDimensions2D
        }
    );
    gFrames.matter1.draw(
        GLSL_PROGRAMS.copy,
        {
            tex: gFrames.extra
        }
    );
    gFrames.forces.draw(
        GLSL_PROGRAMS.initDist,
        {
            sigma: new Vec3(
                sigma.x, sigma.y, sigma.z
            ),
            location: new Vec3(
                position.x, position.y, position.z),
            amplitude: new Vec4(
                force.x, force.y, force.z, 0.0),
            tex: gFrames.velocity1,
            texelDimensions3D: gSimParams.gridDimensions,
            texelDimensions2D: gSimParams.gridDimensions2D
        }
    );
    // gFrames.velocity1.draw(
    //     GLSL_PROGRAMS.copy,
    //     {
    //         tex: gFrames.extra
    //     }
    // );
    // gFrames.forces.draw(GLSL_PROGRAMS.zero, {});
    gSimParams.addForces = true;
}

function addForces(velocity) {
    // console.log(gSimParams.dt);
    // console.log(gFrames.forces);
    gFrames.extra.draw(
        GLSL_PROGRAMS.addForces,
        {
            dt: gSimParams.dt,
            forceTex: gFrames.forces,
            fluidVelocityTex: velocity,
            texelDimensions2D: gSimParams.gridDimensions2D,
            texelDimensions3D: gSimParams.gridDimensions,
        }
    );
    velocity.draw(
        GLSL_PROGRAMS.copy,
        {
            tex: gFrames.extra
        }
    );
    gFrames.forces.draw(GLSL_PROGRAMS.zero, {});
    gSimParams.addForces = false;
}

function advection(dst, src, velocity, dt) {
    dst.draw(
        GLSL_PROGRAMS.advection,
        {
            dt: dt,
            velocityTex: velocity,
            materialTex: src,
            texelDimensions2D: gSimParams.gridDimensions2D,
            texelDimensions3D: gSimParams.gridDimensions,
            dimensions3D: gSimParams.dimensions
        }
    );
}

function advectionHigherOrder(dst, src, velocity) {
    let dt = gSimParams.dt;
    advection(gFrames.advForward, src, velocity, dt);
    advection(gFrames.advReverse, gFrames.advForward, velocity, -dt);
    dst.draw(
        GLSL_PROGRAMS.advectionHigherOrder,
        {
            dt: gSimParams.dt,
            texelDimensions2D: gSimParams.gridDimensions2D,
            texelDimensions3D: gSimParams.gridDimensions,
            dimensions3D: gSimParams.dimensions,
            forwardTex: gFrames.advForward,
            reverseTex: gFrames.advReverse,
            initialTex: src,
            velocityTex: velocity
        }
    );

}

function divergence(divVec, vec) {
    divVec.draw(
        GLSL_PROGRAMS.divergence,
        {
            tex: vec,
            texelDimensions2D: gSimParams.gridDimensions2D,
            texelDimensions3D: gSimParams.gridDimensions,
            dimensions3D: gSimParams.dimensions
        }

    );
}

function computePressure(divergenceVelocity, nIter=10) {
    for (let i = 0; i < nIter; i++) {
        gFrames.pressureIter2.draw(
            GLSL_PROGRAMS.pressureIter,
            {
                toSolveTex: divergenceVelocity,
                lastIterTex: gFrames.pressureIter1,
                texelDimensions2D: gSimParams.gridDimensions2D,
                texelDimensions3D: gSimParams.gridDimensions,
                dimensions3D: gSimParams.dimensions
            }
        );
        [gFrames.pressureIter1, gFrames.pressureIter2] 
            = [gFrames.pressureIter2, gFrames.pressureIter1];
    }
    return gFrames.pressureIter1;
}

function computeDiffusion(material, diffusionCoeff, nIter=10) {
    for (let i = 0; i < nIter; i++) {
        gFrames.diffusionIter2.draw(
            GLSL_PROGRAMS.diffusionIter,
            {
                texelDimensions2D: gSimParams.gridDimensions2D,
                texelDimensions3D: gSimParams.gridDimensions,
                dimensions3D: gSimParams.dimensions,
                diffusionCoeff: diffusionCoeff,
                toSolveTex: material,
                lastIterTex: 
                    (i == 0)?  material: gFrames.diffusionIter1,
                dt: gSimParams.dt
            }
        );
        [gFrames.diffusionIter1, gFrames.diffusionIter2] 
            = [gFrames.diffusionIter2, gFrames.diffusionIter1];
    }
    material.draw(
        GLSL_PROGRAMS.copy, {tex: gFrames.diffusionIter1}
    );
}

function subtractGradPressure(divFreeVelocity, oldVelocity, pressure) {
    divFreeVelocity.draw(
        GLSL_PROGRAMS.subtractGradP,
        {
            pressureTex: pressure,
            fluidVelocityTex: oldVelocity,
            texelDimensions2D: gSimParams.gridDimensions2D,
            texelDimensions3D: gSimParams.gridDimensions,
            dimensions3D: gSimParams.dimensions
        }
    );
}

let gVolRenderNumberOfSlices
    = parseInt(document.getElementById(
        "numberOfSlices"
    ).value);
let gVolRenderSliceWidth
    = parseInt(document.getElementById(
        "sliceSideWidth"
    ).value);
let gVolRender = new VolumeRender(
    new IVec2(gCanvas.height, gCanvas.height),
    new IVec3(gVolRenderSliceWidth, gVolRenderSliceWidth,
                gVolRenderNumberOfSlices)
);

let gIndexOffset = new IVec3(
    gSimParams.gridDimensions.ind[0]/2,
    gSimParams.gridDimensions.ind[1]/2,
    gSimParams.gridDimensions.ind[2]/2,
);

let gPlanarSlices = new PlanarSlices(
    new TextureParams(
        gl.RGBA32F, gCanvas.width, gCanvas.height, true,
        gl.CLAMP_TO_EDGE, gl.CLAMP_TO_EDGE, gl.LINEAR, gl.LINEAR
    )
);


function setPlanarOffset(planarIndex, offset) {
    gIndexOffset.ind[planarIndex] 
            = parseInt(offset*gSimParams.gridDimensions.ind[planarIndex]);
}

function setPlanarOffsetLabel(planarIndex, val) {
    let planarIndicesIds = {
        0: "zIndexLabel", 1: "xIndexLabel", 2: "yIndexLabel"};
    let planarIndicesLabel = {
        0: "xy slice - z offset:",
        1: "yz slice - x offset:",
        2: "zx slice - y offset:"
    };
    document.getElementById(planarIndicesIds[planarIndex]).textContent
        = `${planarIndicesLabel[planarIndex]} `
            + `${parseInt(val*gSimParams.dimensions.ind[planarIndex])}`;
}
setPlanarOffsetLabel(0, 0.5);
setPlanarOffsetLabel(1, 0.5);
setPlanarOffsetLabel(2, 0.5);

document.getElementById("zIndex").addEventListener(
    "input",
    e => {
        let val = parseFloat(e.target.value)/256.0;
        setPlanarOffset(0, val);
        setPlanarOffsetLabel(0, val);
    }
);


document.getElementById("xIndex").addEventListener(
    "input",
    e => {
        let val = parseFloat(e.target.value)/256.0;
        setPlanarOffset(1, val);
        setPlanarOffsetLabel(1, val);
    }
);

document.getElementById("yIndex").addEventListener(
    "input",
    e => {
        let val = parseFloat(e.target.value)/256.0;
        setPlanarOffset(2, val);
        setPlanarOffsetLabel(2, val);
    }
);

document.getElementById("numberOfSlicesLabel").textContent
    = `Number of slices: ${gVolRenderNumberOfSlices}`;
document.getElementById("numberOfSlices").addEventListener(
    "input", e => {
        let numberOfSlices = parseInt(e.target.value);
        let newDimensions = new IVec3(
            gVolRenderSliceWidth, gVolRenderSliceWidth,
            numberOfSlices);
        try {
            let _ = get2DFrom3DDimensions(newDimensions);
        } catch(e) {
            console.log(e);
            return;
        }
        gVolRenderNumberOfSlices = numberOfSlices;
        document.getElementById("numberOfSlicesLabel").textContent
            = `Number of slices: ${gVolRenderNumberOfSlices}`;
        document.getElementById("sliceSideWidthLabel").textContent
            = `Slice size: ${gVolRenderSliceWidth}x${gVolRenderSliceWidth}`;
        gVolRender.resetVolumeDimensions(newDimensions);
    }
);
document.getElementById("sliceSideWidthLabel").textContent
    = `Slice size: ${gVolRenderSliceWidth}x${gVolRenderSliceWidth}`;
document.getElementById("sliceSideWidth").addEventListener(
    "input", e => {
        let volRenderSliceWidth = parseInt(e.target.value);
        let newDimensions = new IVec3(
            volRenderSliceWidth, volRenderSliceWidth,
            gVolRenderNumberOfSlices
        );
        try {
            let _ = get2DFrom3DDimensions(newDimensions);
        } catch(e) {
            console.log(e);
            return;
        }
        gVolRenderSliceWidth = volRenderSliceWidth;
        document.getElementById("numberOfSlicesLabel").textContent
            = `Number of slices: ${gVolRenderNumberOfSlices}`;
        document.getElementById("sliceSideWidthLabel").textContent
            = `Slice size: ${gVolRenderSliceWidth}x${gVolRenderSliceWidth}`;
        gVolRender.resetVolumeDimensions(newDimensions);
    }
);

document.getElementById(
    "useMacCormackSchemeCheckbox").addEventListener(
    "change", e => gSimParams.useMacCormackScheme = e.target.checked
);

function connectSlider(variableName, typeName, variableDisplayText) {
    document.getElementById(`${variableName}SliderLabel`).textContent
         = `${variableDisplayText} = ${gSimParams[variableName]}`;
    document.getElementById(
        `${variableName}Slider`
    ).addEventListener(
        "input",
        e => {
            let val = (typeName === 'int')? 
                parseInt(e.target.value): parseFloat(e.target.value);
            gSimParams[variableName] = val;
            document.getElementById(`${variableName}SliderLabel`).textContent
                = `${variableDisplayText} = ${gSimParams[variableName]}`;
            }
    );
}

connectSlider('dt', 'float', 'Time step');
connectSlider('velocityDiffusionSteps', 'int', 'Iteration steps');
connectSlider('velocityDiffusionStrength', 'float', 'Strength');
connectSlider('materialDiffusionSteps', 'int', 'Iteration steps');
connectSlider('materialDiffusionStrength', 'float', 'Strength');
connectSlider('pressureSteps', 'int', 'Pressure solve step count');

let gStepsPerFrame = Number.parseInt(
    document.getElementById("stepsPerFrame").value);
document.getElementById("stepsPerFrameLabel").textContent 
    = `Updates/frame: ${gStepsPerFrame}`;
document.getElementById("stepsPerFrame").addEventListener(
    "input",
    e => {
        gStepsPerFrame = Number.parseInt(e.target.value);
        document.getElementById("stepsPerFrameLabel").textContent
            = `Updates/frame: ${gStepsPerFrame}`;
    }
);


let gScale = 1.0;

let gUserTime = 0.0;
let gUserDeltaTs = [];
function displayAverageFPS() {
    let time = performance.now();
    let deltaT = (time - gUserTime)/1000.0;
    gUserDeltaTs.push(deltaT);
    let elapsedT;
    // let stopT = 0.5;
    if ((elapsedT = gUserDeltaTs.reduce((a, b) => a + b)) > 0.25) {
        document.getElementById("fps").textContent 
            = `fps: ${ Math.floor(gUserDeltaTs.length/elapsedT)}`;
        gUserDeltaTs = [];
    }
    gUserTime = time;
}

let gRotation = Quaternion.rotator(Math.PI/4.0, 0.0, 0.0, 1.0);
// let gRotation = Quaternion.rotator(0.0, 0.0, 0.0, 1.0);
let gMouseIdlePosition = [0.0, 0.0];
let gMouseInteractPosition = [];
let gMousePosition = [];


function setRotation(x0, y0, x1, y1) {
    let d = new Vec3(x1 - x0, y1 - y0, 0.0);
    let axis = Vec3.crossProd(d, new Vec3(0.0, 0.0, -1.0));
    let angle = 10.0*Math.sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    // console.log(angle, '\naxis: ', axis.x, axis.y, 
    //             '\nquaternion: ', gRotation);
    let rot = Quaternion.rotator(angle, axis.x, axis.y, axis.z);
    gRotation = mul(gRotation, rot);  
}

function getMouseXY(e) {
    let x = (e.clientX - gCanvas.offsetLeft)/gCanvas.width;
    let y = 1.0 - (e.clientY - gCanvas.offsetTop)/gCanvas.height;
    return [x, y];
}

function equalizeXYScaling(xy) {
    let canvasWidth = gCanvas.width;
    let canvasHeight = gCanvas.height;
    let x0 = xy[0], y0 = xy[1];
    if (canvasWidth >= canvasHeight) {
        let offset = ((canvasWidth - canvasHeight)/2.0)/canvasWidth;
        let x1 = (x0 - offset)*canvasWidth/canvasHeight;
        return [x1, y0];
    }
    else {
        let offset = ((canvasHeight - canvasWidth)/2.0)/canvasHeight;
        let y1 = (y0 - offset)*canvasHeight/canvasWidth;
        return [x0, y1];
    }
}

function scaleVolume(scaleVal) {
    gScale -= scaleVal;
    if (gScale < 0.1)
        gScale = 0.1;
    if (gScale > 1.0)
        gScale = 1.0;
}

gCanvas.addEventListener("wheel", e => {
    scaleVolume(e.deltaY/1600.0);
    // let [x, y] = equalizeXYScaling(getMouseXY(e));
    gMouseIdlePosition = equalizeXYScaling(getMouseXY(e));
    // updateTextPosition(x, y);
});

const INPUT_MODES = {NONE: 0, ROTATE_VIEW: 1, MODIFY_FLUID: 2,
                     SKETCH_BARRIER: 3, ERASE_BARRIER: 4};
let gInputMode = INPUT_MODES.ROTATE_VIEW;

function getButtons() {
    let rotateView = document.getElementById("rotateViewButton");
    let addBlob = document.getElementById("addBlobButton");
    return [rotateView, addBlob];
}

function clearButtonStyles(buttons) {
    for (let b of buttons)
        b.style = ``;
}

function applyDrawButtonPressedStyle(drawButtons) {
    let [rotateView, addBlob] = drawButtons;
    let style = `background-color: gray;`;
    switch (gInputMode) {
        case INPUT_MODES.ROTATE_VIEW:
            rotateView.style = style;
            break;
        case INPUT_MODES.MODIFY_FLUID:
            addBlob.style = style;
            break;
        case INPUT_MODES.SKETCH_BARRIER:
            // TODO
            break;
        case INPUT_MODES.ERASE_BARRIER:
            // TODO
            break;
        default:
            break;
    }
}
applyDrawButtonPressedStyle(getButtons());

function manageButtons() {
    let [rotateView, addBlob] = getButtons();
    let buttons = [rotateView, addBlob];
    addBlob.addEventListener(
        "click", () => {
            gInputMode = INPUT_MODES.MODIFY_FLUID;
            clearButtonStyles(buttons);
            applyDrawButtonPressedStyle(buttons);
            refreshSketchSizeDisplay(gSketchWidth, gSketchDepth, gInputMode);
        }
    );
    rotateView.addEventListener(
        "click", () => {
            gInputMode = INPUT_MODES.ROTATE_VIEW;
            clearButtonStyles(buttons);
            applyDrawButtonPressedStyle(buttons);
            refreshSketchSizeDisplay(gSketchWidth, gSketchDepth, gInputMode);
        }
    );
    // TODO: do the rest.
}
manageButtons();

const addBlobSketchWidth
    = sketchWidth => 0.05*sketchWidth/100.0 + 0.005;


let gSketchWidth = document.getElementById("sketchWidth").value;
const sketchDepthFunc
    = sketchDepth => sketchDepth/100.0;

const refreshSketchSizeDisplay = (sketchWidth, sketchDepth, inputMode) => {
    switch(inputMode) {
        case INPUT_MODES.MODIFY_FLUID:
            // TODO!
            // document.getElementById("sketchWidthLabel").textContent
            //     = `Sketch size: ${
            //         (4.0*gSimParams.gridDimensions.ind[0]
            //             *addBlobSketchWidth(sketchWidth)).toFixed(0)
            //     }`;
            // document.getElementById("sketchDepthLabel").textContent
            //     = `Sketch depth: ${
            //         ((gSimParams.gridDimensions.ind[2])
            //             *sketchDepthFunc(sketchDepth)).toFixed(0)
            //     }`;
            break;
        default:
            break;
    }
}

let gSketchDepth = document.getElementById("sketchDepth").value;

document.getElementById("sketchWidth").addEventListener(
    "input",
    e => {
        gSketchWidth = e.target.value;
        refreshSketchSizeDisplay(gSketchWidth, gSketchDepth, gInputMode);
    }
);

document.getElementById("sketchDepth").addEventListener(
    "input",
    e => {
        gSketchDepth = e.target.value;
        refreshSketchSizeDisplay(gSketchWidth, gSketchDepth, gInputMode);
    }
);

function scaleRotate(r) {
    let r2 = sub(r, new Vec3(0.5, 0.5, 0.0));
    let q = Quaternion.rotate(
        new Quaternion(1.0, r2.x, r2.y, r2.z), gRotation.conj());
    return new Vec3(q.i/gScale, q.j/gScale, q.k/gScale);
}

let gColor = null;

function mouseInputFunc(e) {
    if (e.buttons !== 0) {
        if (getMouseXY(e)[0] < 0.0 || getMouseXY(e)[0] > 1.0 ||
            getMouseXY(e)[1] < 0.0 || getMouseXY(e)[0] > 1.0) {
            if (gMousePosition.length >= 0)
                gMousePosition = [];
            return;
        }
        if (gInputMode === INPUT_MODES.ROTATE_VIEW) {
            if (gMousePosition.length === 0) {
                gMousePosition = equalizeXYScaling(getMouseXY(e));
                // let [x1, y1] = equalizeXYScaling(getMouseXY(e));
                // let [x0, y0] = gMouseInteractPosition;
                // setRotation(x0, y0, x1, y1);
                // gMouseInteractPosition = [x1, y1];
                return;
            }
            // console.log(gMousePosition);
            let [x1, y1] = equalizeXYScaling(getMouseXY(e));
            let [x0, y0] = gMousePosition;
            setRotation(x0, y0, x1, y1);
            gMousePosition = [x1, y1];
        }
        else if (gInputMode === INPUT_MODES.MODIFY_FLUID) {
            let [x1, y1] = equalizeXYScaling(getMouseXY(e));
            if (gMousePosition.length === 0) {
                gColor = mul(1.0, new Vec3(Math.random(), Math.random(), Math.random()));
                gMousePosition = [x1, y1];
                // TODO
                return;
            }
            let [x0, y0] = gMousePosition;
            let depth = sketchDepthFunc(gScale*gSketchDepth);
            let r0 = new Vec3(x0, y0, depth);
            let r1 = new Vec3(x1, y1, depth);
            let halfOffset = new Vec3(0.5, 0.5, 0.5);
            r0 = scaleRotate(r0);
            r1 = scaleRotate(r1);
            if (parseInt(gViewMode) === VIEW_MODE.PLANAR_SLICES) {
                let s0 = scaleRotate(new Vec3(x0, y0, depth + 1.0));
                let s1 = scaleRotate(new Vec3(x1, y1, depth + 1.0));
                [r0, r1] = gPlanarSlices.getPositionOnPlanesFor2Lines(
                    r0, r1, s0, s1, gSimParams.gridDimensions, gIndexOffset
                );
            }
            let sigma = new Vec3(
                0.03*gSketchWidth/gSimParams.gridDimensions.ind[0],
                0.03*gSketchWidth/gSimParams.gridDimensions.ind[1],
                0.03*gSketchWidth/gSimParams.gridDimensions.ind[2]);
            let deltaR = sub(r1, r0);
            let force = mul(100.0, deltaR);
            addBlob(add(r0, halfOffset), force, 
                    gColor, sigma);
            gMousePosition = [x1, y1];
            // TODO
        }
        else if (gInputMode === INPUT_MODES.SKETCH_BARRIER) {
            let [x1, y1] = equalizeXYScaling(getMouseXY(e));
            if (gMousePosition.length === 0) {
                gMousePosition = [x1, y1];
            }
            // TODO
        }
        else if (gInputMode === INPUT_MODES.ERASE_BARRIER) {
            let [x1, y1] = equalizeXYScaling(getMouseXY(e));
            if (gMousePosition.length === 0) {
                gMousePosition = [x1, y1];
            }
            // TODO
        }
    }
}

gCanvas.addEventListener("mousemove", e => {
    let [x, y] = equalizeXYScaling(getMouseXY(e));
    gMouseIdlePosition = [x, y];
    // updateTextPosition(x, y);
    mouseInputFunc(e);
});
gCanvas.addEventListener("mousedown", e => mouseInputFunc(e));

gCanvas.addEventListener("mouseup", () => {
    gMousePosition = [];
});


/* function updateTextPosition(x, y) {
    let position0 = new Vec3(x, y, sketchDepthFunc(gScale*gSketchDepth));
    let position = scaleRotate(position0);
    let hoveringElement = document.getElementById("hoveringElements");
    hoveringElement.style = `opacity: 1; `
        + `position: absolute; top: ${parseInt(gCanvas.offsetTop + 0.95*gCanvas.height)}px; `
        + `left: ${gCanvas.offsetLeft + 210}px`;
    let hoveringStats = document.getElementById("hoveringStats");
    hoveringStats.textContent = 
        `x: ${parseInt((position.x)*gSimParams.dimensions.x)}, `
        + `y: ${parseInt((position.y)*gSimParams.dimensions.y)}, `
        + `z: ${parseInt((position.z)*gSimParams.dimensions.z)}`;
}*/

function animation() {
    displayAverageFPS();
    for (let i = 0; i < gStepsPerFrame; i++) {
        if (gSimParams.useMacCormackScheme) {
            advectionHigherOrder(gFrames.matter2,
                    gFrames.matter1, gFrames.velocity1);
            [gFrames.matter1, gFrames.matter2]
                = [gFrames.matter2, gFrames.matter1];
            advectionHigherOrder(gFrames.velocity2,
                    gFrames.velocity1, gFrames.velocity1);
        } else {
            advection(
                gFrames.matter2, gFrames.matter1, 
                gFrames.velocity1, gSimParams.dt);
            [gFrames.matter1, gFrames.matter2]
                = [gFrames.matter2, gFrames.matter1];
            advection(
                gFrames.velocity2, gFrames.velocity1,
                gFrames.velocity1, gSimParams.dt);
        }
        if (gSimParams.addForces)
            addForces(gFrames.velocity2);
        if (gSimParams.velocityDiffusionSteps > 0
            && gSimParams.velocityDiffusionStrength > 0.0)
            computeDiffusion(
                gFrames.velocity2, gSimParams.velocityDiffusionStrength,
                gSimParams.velocityDiffusionSteps);
        if (gSimParams.materialDiffusionSteps > 0
            && gSimParams.materialDiffusionStrength > 0.0)
            computeDiffusion(gFrames.matter1, 
                gSimParams.materialDiffusionStrength,
                gSimParams.materialDiffusionSteps);
        divergence(gFrames.divergenceVelocity, gFrames.velocity2);
        let pressure = computePressure(
            gFrames.divergenceVelocity, gSimParams.pressureSteps);
        subtractGradPressure(gFrames.velocity1, gFrames.velocity2, pressure);
    }
    gFrames.extra.draw(
        GLSL_PROGRAMS.densityView,
        {
            // tex: gFrames.pressureIter1,
            tex: gFrames.matter1,
            colorBrightness: 1.0,
            alphaBrightness: 2.0
        }

    );
    // let arr = gFrames.extra.asFloat32Array();
    // console.log(arr[1000]);
    let view;
    switch (parseInt(gViewMode)) {
        case VIEW_MODE.PLANAR_SLICES:
            let rotation = gRotation;
            let scale = gScale;
            if (gMouseIdlePosition.length !== 0) {
                let [x, y] = gMouseIdlePosition;
                let depth = sketchDepthFunc(gScale*gSketchDepth);
                let position0 = new Vec3(x, y, depth);
                let position = scaleRotate(position0);
                let position1 = scaleRotate(new Vec3(x, y, depth + 1));
                let ray = [position, position1];
                view = gPlanarSlices.view(
                    gFrames.extra, rotation, scale,
                    gIndexOffset.ind[0],
                    gIndexOffset.ind[1],
                    gIndexOffset.ind[2],
                    gSimParams.gridDimensions,
                    ray
                );
            }
            break;
        case VIEW_MODE.VOLUME_RENDER:
            view = gVolRender.view(gFrames.extra, gScale, gRotation);
            break;
    }

    gFrames.target.draw(
        GLSL_PROGRAMS.copy,
        {
            tex: view
        }
    );
    requestAnimationFrame(animation);
}

requestAnimationFrame(animation);