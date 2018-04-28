import { Terrain } from "./terrain";
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";
import { GerstnerWave } from "./gerstnerWave";
import { Communicators, Communicator } from "../gl/communicator";
import { ulist_elem } from "../util/ulist";
import Material from "../gl/material";


class Ocean extends Terrain {
	render(viewport: Viewport, shader: Shader) {
		this.oceanShader.use();
			this.setBasicUniforms(viewport);
			this.waves.invoke();
			this.material.use();
			this.mesh.bind();
				this.mesh.draw();
			this.mesh.unbind();
		this.oceanShader.unuse();
	}
	
	add(wave: GerstnerWave): ulist_elem<Communicator> {
		return this.waves.add(wave);
	}

	private oceanShader = Shader.create("ocean");
	private waves: Communicators = new Communicators();
	private material: Material = new Material({
		specularPower: 0.5,
		specularIntensity: 0.56
	});
}

export {
	Ocean
}
