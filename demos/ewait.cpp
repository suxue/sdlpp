EventData e;
while (true) {
    event::wait(e);
    if (e.getType() == EventType::Quit) {
        break;
    } else if (e.getType() == EventType::Window) {
        auto p = e.acquire<EventType::Window>();
        if (p.getID() == SDL_WINDOWEVENT_EXPOSED) { // redraw
            renderer.present();
        }
    }
}
