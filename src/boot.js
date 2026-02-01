// Math.ceil(1 + BS / 16 / 1024) = 7
// U = Uint8Array;
//console.log(inst.exports)
let _D = document,
    _C = _D.write(`<canvas id=c></canvas>`),
    _T, S = 150, BS = S * S * 4,
    _E = inst.exports,
    computingFlag = 0,
    image = (_T = (_C = _D.getElementById(`c`, _E.memory.grow(7 /*Math.ceil(1 + BS / 16 / 1024)*/)))
        .getContext(`2d`))
        .getImageData(0, 0, S, _C.width = _C.height = S),
    pixPtr = _E.__heap_base.value, // __heap_base .. __heap_end
    pixBuf = new U(mem = _E.memory.buffer).subarray(pixPtr, pixPtr + BS),
    // рендер
    processFrame = (_) => {
        if (computingFlag) { requestAnimationFrame(processFrame); return; }
        _E.R(pixPtr)
        // const ret = _E.render(t, pixPtr)
        // console.log(ret)
        image.data.set(pixBuf)
        _T.putImageData(image, 0, 0)
        requestAnimationFrame(processFrame);
    },    
    WH90 = `min(90vw,90vh)`;
_C.style = `width:${WH90};height:${WH90};image-rendering:pixelated`

// ---
// 60 fps 1000/60=16
// 45 fps 1000/45=22
// 30 fps 1000/30=33
_E.I(33)
// обработка физики
setInterval((_) => { computingFlag = 1; _E.P(); computingFlag = 0 }, 33 /*1000/60=16*/);
// ввод
_D.onmousedown = _D.onkeydown = (_) => _E.K()
processFrame(0)