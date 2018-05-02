import Obj from "./object"
import { Viewport } from "../gl/viewport";
import { Shader } from "../gl/shader";

export default abstract class MeshObj extends Obj {

	abstract render(viewport: Viewport);
}

