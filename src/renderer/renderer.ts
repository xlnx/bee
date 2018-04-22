import { ulist_elem, ulist } from "../util/ulist"
import { Event, EventList } from "./events"
// import { KeyEventType, KeyEvent } from "./keyEvent"

let gl: WebGL2RenderingContext

type RendererEventType = "render" | DocumentEventMap // | KeyEventType

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

	constructor(canvas: HTMLCanvasElement) {
		if (Renderer.context != null) {
			throw "Multiple WebGL contexts not supported.";
		} else {
			try {
				gl = <WebGL2RenderingContext>canvas.getContext("webgl2");// || canvas.getContext("experimental-webgl");
				if (!gl) {
					throw "";
				}
			} catch (e) {
				console.error("webgl 2.0 is not supported. " + e);
				throw "webgl 2.0 is not supported. " + e;
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
	RendererEvent,
	Renderer
}