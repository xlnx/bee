import { ulist_elem, ulist } from "../util/ulist"
import { Event, EventList } from "./events"

let gl: WebGLRenderingContext
let gl2: WebGL2RenderingContext

type RendererEventType = "render" | DocumentEventMap

interface RendererEvent {
	cancel: () => void;
}

class RenderEvent {
	constructor(private elem: ulist_elem<any>)
	{
	}
	cancel() {
		this.elem.remove();
	}
}

class Renderer {
	private static context: Renderer = null;

	private events = new EventList();

	private renderers = new ulist<() => void>();

	constructor(canvas: HTMLCanvasElement, fallback: boolean = false) {
		if (Renderer.context != null) {
			throw "Multiple WebGL contexts not supported.";
		} else {
			try {
				if (!fallback) {
					gl2 = <WebGL2RenderingContext>canvas.getContext("webgl2");// || canvas.getContext("experimental-webgl");
				}
				if (!gl2) {
					gl = <WebGLRenderingContext>canvas.getContext("webgl") 
						|| canvas.getContext("experimental-webgl");
					if (!gl) {
						throw "";
					} else {
						console.warn("webgl 2.0 is not supported on your browser, fallback to webgl 1.0.");
					}
				} else {
					gl = gl2;
				}
			} catch (e) {
				console.error("webgl is not supported on your browser. " + e);
				throw "webgl is not supported on your browser. " + e;
			}
		}
		Renderer.context = this;
	}

	get instance(): Renderer {
		return Renderer.context;
	}
	dispatch(eventType: RendererEventType, callback: any): RendererEvent {
		if (eventType == "render") {
			return new RenderEvent(this.renderers.push(callback));
		} else {
			return this.events.addEvent(eventType, callback);
		}
	}
	render() {
		this.renderers.visit((e: ulist_elem<() => void>) => {
			try {
				e.get()();
			} catch (err) {
				console.error(err);
			}
		});
		setTimeout(() => {
			this.render();
		}, 0);
	}
}

export {
	gl,
	gl2,
	RendererEvent,
	Renderer
}
