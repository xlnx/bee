import { glm } from "../util/glm"
import { VAO, VertexAttrs } from "../gl/vertexAttrs";
import { Renderer } from "../renderer/renderer";
import { Shader } from "../gl/shader";

class Rect {
	private vao: VAO;
	private static shader: Shader;

	public displacement = glm.vec2(0);

	constructor(opts: { [key: string]: any }) {
		let attrs = new VertexAttrs(["pos2", "tex2"]);
		let w = Renderer.instance.canvas.width;
		let h = Renderer.instance.canvas.height;
		Rect.shader = Rect.shader || Shader.create("rect", false);
		attrs.set("pos2", [
			opts.left / w * 2 - 1, 1 - opts.top / h * 2,
			(opts.left + opts.width) / w * 2 - 1, 1 - opts.top / h * 2,
			(opts.left + opts.width) / w * 2 - 1, 1 - (opts.top + opts.height) / h * 2,
			opts.left / w * 2 - 1, 1 - (opts.top + opts.height) / h * 2,
		]);
		attrs.set("tex2", [
			opts.tex[0], opts.tex[1], 
			opts.tex[2], opts.tex[3], 
			opts.tex[4], opts.tex[5], 
			opts.tex[6], opts.tex[7]
		]);
		this.vao = new VAO(attrs, [
			0, 1, 2, 0, 2, 3
		]);
	}

	render() {
		this.gDisplacement.set(this.displacement);
		this.vao.bind();
			this.vao.draw();
		this.vao.unbind();
	}

	static bindShader() {
		!Rect.shader || Rect.shader.use();
	}

	static unbindShader() {
		!Rect.shader || Rect.shader.unuse();
	}

	private gDisplacement = Shader.uniform("vec2", "gDisplacement");
}

export {
	Rect
}