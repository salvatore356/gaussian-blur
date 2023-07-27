import { useMemo, useRef, useState } from "react";
import * as THREE from 'three';
import { Canvas, useFrame } from '@react-three/fiber';
import { Html } from '@react-three/drei';

import vertexShader from './vertexShader';
import fragmentShader from './fragmentShader';

import { useControls } from 'leva';

function GaussPlane() {
  
  const mesh = useRef();
  
  let iChannel0URL = require("./donut.jpg")
  
  const options = useMemo(() => {
    return {
      kernelSize: { 
        value: 5, min: 3, max: 15, step: 2,
        onChange: (value) => {
          let newValue = Math.ceil(value/2);
          console.log('newValue', newValue)
          uniforms.radius.value = newValue;
          
          let temp = [];
          for(let y = -newValue; y < newValue - 1; ++y){
            let line = []
            for(let x = -newValue; x < newValue - 1; ++x)
              line.push(gaussianWeight(y*y + x*x, newValue))
            temp.push(line)
          }
          setKernel(temp)
        }
      },
      hideKernel: {
        value: true
      },
      hideImage: {
        value: false
      },
      hideText: {
        value: true
      }
    }
  })

  const controls = useControls('Gaussian blur', options)

  const [kernel, setKernel] = useState(
    [
      [0.2, 0.4, 0.2],
      [0.4, 1.0, 0.4],
      [0.2, 0.4, 0.2],
    ]
  );

  const [iChannel0] = useMemo(() => {
    const loader = new THREE.TextureLoader();
    return [loader.load(iChannel0URL)];
  }, [iChannel0URL]);

  const uniforms = {
    iTime: {
      value: 0.0,
    },
    iResolution: {
      value : new THREE.Vector2(window.innerWidth, window.innerHeight)
    },
    iChannel0: {
      value: iChannel0
    },
    radius: {
      value: 5
    }
  };

  
  const gaussianWeight = (squaredL2Norm, sigma) => {
    let sigmaSquared = sigma * sigma;
    let factor = 1.0 / (Math.sqrt(2.0 * Math.PI) * sigmaSquared);
    return factor * Math.exp(-squaredL2Norm / (2.0 * sigmaSquared));
}

  useFrame((state) => {
    const { clock } = state;
    mesh.current.material.uniforms.iTime.value = clock.getElapsedTime();
  });

  return (
    <>
    
      <mesh 
        visible={!controls.hideImage}
        ref={mesh} position={[0, 0, 0]} scale={1}>
        <planeGeometry args={[10, 10]} />
        <shaderMaterial
          fragmentShader={fragmentShader}
          vertexShader={vertexShader}
          uniforms={uniforms}
          side={THREE.DoubleSide}
        />
        
      </mesh>
      
        <Html 
          style={{
            visibility: controls.hideKernel? 'hidden': 'visible'
          }}
          as="div" 
          className="kernels-container"
          center
        >
          <h3>
            Kernel
          </h3>
          {
            kernel.map((rows, x) => {
              if (controls.hideKernel) return (<div></div>);
              
              let mid = Math.ceil(kernel.length  / 2.0) - 1
              
              return <div key={x} className="kernels" >{
                
                rows.map((cols, y) => {
                  let opacity = ((1.0 - Math.abs(mid - x) / (mid )) + 
                                (1.0 - Math.abs(mid - y) / (mid ))) / 2;

                  return <div 
                    key={y} 
                    style={{
                      display: 'inline', 
                      padding: 2,
                      opacity: opacity,
                    }}
                  >
                    <span
                      style={{
                        visibility: (controls.hideText ) ? 'hidden': 'visible',
                      }}
                    >
                      {cols.toFixed(4)}
                    </span>
                  </div>
                })

              }</div>
              
            })
          }
        </Html>
      
    
    </>
  );
  
}

export default function GaussianBlur() {

  

  return (
    <Canvas
      dpr={window.devicePixelRatio} 
      camera={{ position: [0.0, 0.0, 10.0] }}
    >
      <GaussPlane />
    </Canvas>

  );
}