import THREE from '../lib/three';
import Renderer from '../lib/renderer';

import vert from '../shaders/extra/test.vert';
import frag from '../shaders/extra/test.frag';
import vert2 from '../shaders/extra/test2.vert';
import frag2 from '../shaders/extra/test2.frag';

import texture from '../textures/mars.png';
import texture2 from '../textures/mimas.png';

export default class extends Renderer {
  initScene() {
    if (!this.checkShader(vert, frag)) {
      this.setErrorScene();
      return;
    }

    this.setLight();

    const textureDisplacement = new THREE.TextureLoader().load(texture);
    this.uniforms['textureDisplacement'] = {
      type: "t",
      value: textureDisplacement
    };
    this.uniforms['textureDimension'] = {
      type: 'vec2',
      value: new THREE.Vector2(100.0, 100.0)
    };

    const geometry = new THREE.SphereBufferGeometry(5, 256, 256);
    const geometry2 = new THREE.SphereBufferGeometry(1, 64, 256);
    const material = this.createShaderMaterial(vert, frag);
    const material2 = this.createShaderMaterial(vert2, frag2);
    
    geometry.computeTangents();
    geometry2.computeTangents();
    const object = new THREE.Mesh(geometry, material);
    const object2 = new THREE.Mesh(geometry2, material2);
    this.scene.add(object);
    this.scene.add(object2);
  }

  update(dt) {
    if (!this.focussed) {
      this.updateCamera(dt / 12000);
    }
  }
}