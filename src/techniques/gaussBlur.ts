import { Shader } from "../gl/shader";
import { glm } from "../util/glm"
import { Renderer, gl } from "../renderer/renderer";
import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { Offscreen } from "../gl/offscreen";
import { Texture2D } from "../gl/texture";

class GaussBlur {
	private static vert: Shader;
	private static hori: Shader;

	private radius: number;
	protected vao: VAO;
	private swapImage: Texture2D = new Texture2D();

	private offscreen = new Offscreen;

	constructor() {
		GaussBlur.vert || (GaussBlur.vert = Shader.create("gaussBlurVert", false));
		GaussBlur.hori || (GaussBlur.hori = Shader.create("gaussBlurHori", false));
		let vertices = new VertexAttrs(["pos2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}

	setRadius(radius: number) {
		this.radius = radius;
	}

	render(texture: Texture2D, dest: Texture2D) {
		this.offscreen.bind();
		this.vao.bind();
			this.offscreen.set(gl.COLOR_ATTACHMENT0, this.swapImage);
			texture.use("Image");
				GaussBlur.vert.use();
				this.gRange.set(this.radius);
				this.iResolution.set(glm.vec2(Renderer.instance.canvas.width,
					Renderer.instance.canvas.height));
				this.vao.draw();
				GaussBlur.vert.unuse();
			texture.unuse();
			this.offscreen.set(gl.COLOR_ATTACHMENT0, dest);
			this.swapImage.use("Image");
				GaussBlur.hori.use();
				this.gRange.set(this.radius);
				this.iResolution.set(glm.vec2(Renderer.instance.canvas.width,
					Renderer.instance.canvas.height));
				this.vao.draw();
				GaussBlur.hori.unuse();
			this.swapImage.unuse();
		this.vao.unbind();
		this.offscreen.unbind();
	}

	private gRange = Shader.uniform("int", "gRange");
	private iResolution = Shader.uniform("vec2", "iResolution");
}

export {
	GaussBlur
}
