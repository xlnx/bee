import Obj from "./object"
import { Viewport } from "./viewport";
import { ulist } from "../util/ulist"
import { Communicators } from "../gl/communicator";

abstract class Technique {
	abstract render(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators);
	abstract mainImage(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators);
}

export {
	Technique
}
