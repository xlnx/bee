import Obj from "../scene/object"
import { Viewport } from "../scene/viewport";
import { Shader } from "../gl/shader";

export default abstract class MeshObj extends Obj {

	abstract render(viewport: Viewport);
}

