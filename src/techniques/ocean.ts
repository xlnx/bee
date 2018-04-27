import { Terrain } from "./terrain";
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";


class Ocean extends Terrain {
	private gSpecularIntensity = Shader.uniform("float", "gSpecularIntensity");

	render(viewport: Viewport, shader: Shader) {
		this.oceanShader.use();
			this.setViewMatrices(viewport);
			this.gSpecularIntensity.set(0.5);
			this.mesh.bind();
				this.mesh.draw();
			this.mesh.unbind();
		this.oceanShader.unuse();
	}

	private oceanShader = Shader.create("ocean");
}

export {
	Ocean
}
