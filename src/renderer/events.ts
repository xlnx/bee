import { ulist_elem, ulist } from "../util/ulist"

class Event {
	constructor(private event: any, private elem: ulist_elem<any>)
	{
	}
	cancel() {
		document.removeEventListener(this.event, this.elem.get());
		this.elem.remove();
	}
}

class EventList {
	private events = new ulist<any>();

	addEvent(event: any, callback: any): Event {
		document.addEventListener(event, callback);
		return new Event(event, this.events.push(callback));
	}
}

export {
	Event,
	EventList
}
