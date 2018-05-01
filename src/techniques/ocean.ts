import { Terrain } from "./terrain";
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "./gerstnerWave";
import { Communicators, Communicator } from "../gl/communicator";
import { ulist_elem } from "../util/ulist";
import Material from "../gl/material";


class Ocean extends Terrain {
	constructor() {
		super();
		this.defaultShader = Shader.create("ocean", true);
	}

	render(viewport: Viewport) {
		this.setBasicUniforms(viewport);
		this.waves.invoke();
		this.material.use();
		this.mesh.bind();
			this.mesh.draw();
		this.mesh.unbind();
	}
	
	add(wave: GerstnerWave): ulist_elem<Communicator> {
		return this.waves.add(wave);
	}

	private waves: Communicators = new Communicators();
	private material: Material = new Material({
		specularPower: 0.5,
		specularIntensity: 0.56
	});
}

export {
	Ocean
}
