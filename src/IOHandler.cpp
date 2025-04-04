#include "pch.hpp"

#include "IOHandler.hpp"

#include "MyApp.hpp"

#include "ShaderManager.hpp"
#include "MyFrame.hpp"

#include "Renderer.hpp"
#include "RenderTextUtility.hpp"

#include "DataResourceManager.hpp"

void IOHandler::OnCanvasSizeChange() {
	Renderer& r = *Frame->renderer;

	r.UpdateSize();
}

void IOHandler::OnDelete() {
	Renderer& r = *Frame->renderer;
	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
	if (state == State::Marking) {
		SetState(State::Normal);

		b.DeleteMarked();
		r.Dirty = true;
		r.Render();
		/*unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
		if (h != 0) {
			b.SetHighlited(h);
			r.Dirty = true;
			r.Render();
		}*/
		return;
	}
	if (b.HasHighlited()) {
		b.SetMarked(b.GetHighlited(), true);
		b.DeleteMarked();
		r.Dirty = true;
		r.Render();
		return;
	}
}

Eigen::Vector2f IOHandler::PixelToScreenCoord(const wxPoint& Pos) {
	Renderer& r = *Frame->renderer;

	Eigen::Vector3f NormalizedMouse{
	(Pos.x / r.CanvasSize.x() - 0.5f) * 2.0f,
	((r.CanvasSize.y() - Pos.y) / r.CanvasSize.y() - 0.5f) * 2.0f,
	1.0 };

	Eigen::Vector3f Product = r.ViewProjectionMatrix * NormalizedMouse;
	Eigen::Vector2f Transformed = Product.head<2>();
	return Transformed;
}

//Returnes if the index changed
bool IOHandler::UpdateMouseIndex(const wxPoint& Pos) {

	Renderer& r = *Frame->renderer;

	auto LastMouseIndex = MouseIndex;
	MouseIndex = r.CoordToNearestPoint(PixelToScreenCoord(Pos));

	/*if (state == State::Dragging) {
		MouseIndex = { 0, 0 };
	}*/

	if (LastMouseIndex == MouseIndex)return false;

	r.UpdateMouseIndex(MouseIndex);

	return true;
}

IOHandler::IOHandler(MyFrame* Frame) : Frame(Frame) {}

void IOHandler::MoveViewUpdate() {
	const Eigen::Vector2f Speed = Keyboarddata.Shift ? Eigen::Vector2f{ 100, 100 } : Eigen::Vector2f{ 25, 25 };
	Eigen::Vector2f Diff{ 0,0 };

	if (Keyboarddata.W) Diff.y() -= Speed.y();
	if (Keyboarddata.A) Diff.x() += Speed.x();
	if (Keyboarddata.S) Diff.y() += Speed.y();
	if (Keyboarddata.D) Diff.x() -= Speed.x();

	Renderer& r = *Frame->renderer;

	if (Diff != Eigen::Vector2f::Zero()) {
		r.Dirty = true;
	}
	r.Offset += 2.0 * Diff * r.Zoom;

	if (Keyboarddata.E) {

		Eigen::Vector2f Centre = r.CanvasSize / 2.0;

		Eigen::Vector2f PosD = Centre - Keyboarddata.MousePosition;
		PosD.y() *= -1;

		r.Offset += 2.0 * PosD * r.Zoom;

		// Zoom anpassen
		if (!Keyboarddata.Shift) {
			r.Zoom *= 0.9f;
		}
		else {
			r.Zoom *= 1.1f;
		}

		r.CheckZoomDots();

		r.Offset -= 2.0 * PosD * r.Zoom;
		r.Dirty = true;
	}

	if (Keyboarddata.W || Keyboarddata.A || Keyboarddata.S || Keyboarddata.D || Keyboarddata.E) {
		r.UpdateViewProjectionMatrix();
		UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });

		r.Render();

		if (state == State::Normal || state == State::Marking) {
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			if (b.SetHighlited(r.GetHighlited({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() }))) {
				r.Dirty = true;
				r.Render();
			}
		}
	}


	/*if (Keyboarddata.Shift && state == Deleting) {
		DeleteUnderCursor();
	}*/
}

void IOHandler::SetState(const State& state) {
	//if (this->state == State::Placing) {
		//NextPlaceBuilding.reset();
	//}
	if (state == State::GoHome) {
		GoHomeSaveState = this->state;
	}
	this->state = state;
	if (this->state == State::GoHome) {
		Renderer& r = *Frame->renderer;
		r.StartGoHome();
	}
	//if (this->state == Placing) {
		//BottomupMenu.Show(true);
	//}
}

std::string IOHandler::GetStateString() const {
	using enum State;
	switch (state) {
	case Normal:
		return "Normal";
	case Dragging:
		return "Dragging";
	case PlacingLine:
		return "PlacingLine";
	case DraggingWhilePlacingLine:
		return "DraggingWhilePlacingLine";
	case Marking:
		return "Marking";
	case DraggingWithMarking:
		return "DraggingWithMarking";
	case DraggingMarked:
		return "DraggingMarked";
	case AreaFirstPoint:
		return "AreaFirstPoint";
	case DraggingWhileAreaFistPoint:
		return "DraggingWhileAreaFistPoint";
	case GoHome:
		return "GoHome";
	}
	return "Unknown";
}

void IOHandler::DoLoop() {
	PROFILE_FUNKTION;

	Renderer& r = *Frame->renderer;

	if (state == State::GoHome) {
		if (r.StepGoHome()) {
			SetState(GoHomeSaveState);
			return;
		}
	}

	MoveViewUpdate();

	r.Render();
}

void IOHandler::OnMouseWheel(const wxPoint& wxPos, double Rotation) {

	Renderer& r = *Frame->renderer;

	Eigen::Vector2f Pos = { wxPos.x,wxPos.y };
	Eigen::Vector2f Centre = r.CanvasSize / 2.0;

	Eigen::Vector2f PosD = Centre - Pos;
	PosD.y() *= -1;

	r.Offset += 2.0 * PosD * r.Zoom;

	r.Zoom *= std::pow(1.1f, -Rotation / 60.0f);

	r.CheckZoomDots();

	r.Offset -= 2.0 * PosD * r.Zoom;

	// Matrix aktualisieren
	r.UpdateViewProjectionMatrix();
	UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });

	r.Dirty = true;
	r.Render();
}

void IOHandler::OnMouseMove(const wxPoint& wxPos) {
	/*
	{ -1.0, -1.0 },
	{ -1.0,1.0 },
	{ 1.0,-1.0 },
	{ 1.0,1.0 },

	*/

	Keyboarddata.MousePosition = { wxPos.x,wxPos.y };
	//SetTitle(ss.str());

	if (state == State::Normal || state == State::Marking) {
		Renderer& r = *Frame->renderer;
		VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
		if (b.SetHighlited(r.GetHighlited(Keyboarddata.MousePosition))) {
			r.Dirty = true;
			r.Render();
		}
	}

	if (state == State::DraggingMarked) {
		Click = false;
		Eigen::Vector2i CurrentMouseIndex = MouseIndex;
		if (UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() })) {
			Eigen::Vector2i Diff = MouseIndex - CurrentMouseIndex;
			Renderer& r = *Frame->renderer;
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			b.MoveMarked(Diff);
			r.Dirty = true;
			r.Render();
		}
		return;
	}

	if (state == State::AreaFirstPoint) {

		AreaSecondPoint = PixelToScreenCoord(wxPos);

		VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
		Renderer& r = *Frame->renderer;

		auto& buff = r.GetAreaSelectVerts();
		buff.Clear();
		buff.Emplace(AreaFirstPoint, AreaSecondPoint, ColourType{ 1.0,0.0,1.0,0.1 });

		b.MarkArea(MyRectF::FromCorners(AreaFirstPoint, AreaSecondPoint));

		r.Dirty = true;
		r.Render();
	}

	if (state == State::Dragging || state == State::DraggingWhilePlacingLine || state == State::DraggingWithMarking || state == State::DraggingWhileAreaFistPoint) {
		Click = false;
		Renderer& r = *Frame->renderer;

		wxPoint LastOrigin = DraggingOrigin;
		DraggingOrigin = wxPos;

		wxPoint Diff = DraggingOrigin - LastOrigin;
		r.Offset.x() += 2.0 * Diff.x * r.Zoom;
		r.Offset.y() -= 2.0 * Diff.y * r.Zoom;

		r.UpdateViewProjectionMatrix();
		UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });
		r.Dirty = true;
		r.Render();
		return;
	}

	UpdateMouseIndex(wxPos);
}

void IOHandler::OnMouseDown(const wxPoint& wxPos) {
	//Beep(200, 200);

	//if (ClickMenu(wxPos))return;

	//PlaceNextPlaceBuilding();
	//DeleteUnderCursor();

	UpdateMouseIndex(wxPos);
	if (state == State::PlacingLine) {
		SetState(State::DraggingWhilePlacingLine);
	}
	else if (state == State::Marking) {
		Renderer& r = *Frame->renderer;
		VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
		unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
		if (b.GertMarked(h)) {
			SetState(State::DraggingMarked);
		}
		else {
			SetState(State::DraggingWithMarking);
		}
	}
	else if (state == State::AreaFirstPoint) {
		SetState(State::DraggingWhileAreaFistPoint);
	}
	else {
		SetState(State::Dragging);
	}
	Click = true;
	DraggingOrigin = wxPos;
}

void IOHandler::OnMouseUp(const wxPoint& wxPos) {
	StoppDragg();
	if (Click) {
		Click = false;
		//Beep(600, 200);
		Renderer& r = *Frame->renderer;
		VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;

		if (state == State::Normal) {
			unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
			if (h != 0) {
				b.SetMarked(h, true);
				r.Dirty = true;
				r.Render();

				SetState(State::Marking);
				return;
			}

			SetState(State::PlacingLine);
			Frame->BlockManager->CurrentInterior->StartDrag(MouseIndex);
			return;
		}
		if (state == State::Marking) {
			unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
			if (h != 0) {
				b.SetMarked(h, !b.GertMarked(h));
			}
			else {
				b.ClearMarked();
			}
			r.Dirty = true;
			r.Render();
			return;
		}
		if (state == State::AreaFirstPoint) {
			AreaSecondPoint = PixelToScreenCoord(wxPos);
			b.MarkArea(MyRectF::FromCorners(AreaFirstPoint, AreaSecondPoint));

			SetState(State::Marking);
			AreaFirstPoint = {};

			r.GetAreaSelectVerts().Clear();
			r.Dirty = true;
			r.Render();
			return;
		}
		if (state == State::PlacingLine) {
			if (Frame->BlockManager->CurrentInterior->AddDrag(MouseIndex)) {
				Frame->BlockManager->CurrentInterior->EndDrag();
				SetState(State::Normal);
			}
			r.Dirty = true;
			r.Render();
			return;
		}
	}
}


void IOHandler::OnRightMouseDown(const wxPoint& wxPos) {
	UpdateMouseIndex(wxPos);
	Renderer& r = *Frame->renderer;
	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
	if (state == State::Marking || state == State::DraggingMarked || state == State::DraggingWithMarking) {
		b.ClearMarked();
	}
	if (state == State::PlacingLine || state == State::DraggingWhilePlacingLine) {
		b.CancleDrag();
	}
	DraggingOrigin = { 0,0 };
	SetState(State::AreaFirstPoint);
	AreaFirstPoint = PixelToScreenCoord(wxPos);
	/*if (state == State::PlacingLine) {
		SetState(State::DraggingWhilePlacingLine);
	}
	else if (state == State::Marking) {
		Renderer& r = *Frame->renderer;
		VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
		unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
		if (b.GertMarked(h)) {
			SetState(State::DraggingMarked);
		}
		else {
			SetState(State::DraggingWithMarking);
		}
	}
	else {
		SetState(State::Dragging);
	}*/
	Click = true;
	DraggingOrigin = wxPos;
}

void IOHandler::OnDClick(const wxPoint& wxPos) {
	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
	if (state == State::PlacingLine && b.GetDragSize() == 1) {
		b.CancleDrag();
		//SetState(State::AreaFirstPoint);
		Beep(200, 200);
	}
}

void IOHandler::StoppDragg() {
	if (state == State::Dragging) {
		SetState(State::Normal);
	}
	else if (state == State::DraggingWithMarking) {
		SetState(State::Marking);
	}
	else if (state == State::DraggingMarked) {
		SetState(State::Marking);
	}
	else if (state == State::DraggingWhilePlacingLine) {
		SetState(State::PlacingLine);
	}
	else if (state == State::DraggingWhileAreaFistPoint) {
		SetState(State::AreaFirstPoint);
	}
	DraggingOrigin = { 0,0 };
	UpdateMouseIndex({ (int)Keyboarddata.MousePosition.x(),(int)Keyboarddata.MousePosition.y() });
}
//insert event
void IOHandler::OnKeyDown(wxKeyEvent& evt) {

	VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
	Renderer& r = *Frame->renderer;


	Keyboarddata.Shift = evt.ShiftDown();
	Keyboarddata.Strng = evt.CmdDown();
	switch (evt.GetKeyCode()) {
	case 'W':
		Keyboarddata.W = true;
		break;
	case 'A':
		if (Keyboarddata.Strng) {
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			Renderer& r = *Frame->renderer;
			if (state == State::PlacingLine || state == State::DraggingWhilePlacingLine) {
				b.EndDrag();
			}

			SetState(State::Marking);
			b.MarkAll();
			r.Dirty = true;
			r.Render();
			break;
		}
		Keyboarddata.A = true;
		break;
	case 'S':
		Keyboarddata.S = true;
		break;
	case 'D':
		Keyboarddata.D = true;
		break;
	case 'E':
		Keyboarddata.E = true;
		break;
	/*case 'F':
		Keyboarddata.F = true;
		break;*/
	case 'R':
	{
		b.RotateMarked();
		if (!Keyboarddata.Shift) {
			b.RotateCW({ 0,0 });
		}
		else {
			b.RotateCCW({ 0,0 });
		}
		r.Dirty = true;
		r.Render();
	}
	break;
	case 'V':
		b.FlipMarkedY();
		r.Dirty = true;
		r.Render();
		break;
	case 'H':
		b.FlipMarkedX();
		r.Dirty = true;
		r.Render();
		break;
	case 'O':
	{
		SetState(State::GoHome);
	}
	break;
	}
}

void IOHandler::OnKeyUp(wxKeyEvent& evt) {

	Keyboarddata.Shift = evt.ShiftDown();
	Keyboarddata.Strng = evt.CmdDown();
	switch (evt.GetKeyCode()) {
	case 'W':
		Keyboarddata.W = false;
		break;
	case 'A':
		Keyboarddata.A = false;
		break;
	case 'S':
		Keyboarddata.S = false;
		break;
	case 'D':
		Keyboarddata.D = false;
		break;
	case 'E':
		Keyboarddata.E = false;
		break;
	/*case 'F':
		Keyboarddata.F = false;
		break;*/
	}
}

void IOHandler::OnChar(wxKeyEvent& evt) {

	/*auto SelectNthButton = [this](const size_t n) {
		BottomupMenu.Show(true);
		if (BottomupMenu.SelectButtonByIndex(n)) {
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
		}
		};*/

	switch (evt.GetKeyCode()) {
	case WXK_DELETE:
		OnDelete();
		break;
	case WXK_ESCAPE:
		if (state == State::PlacingLine) {
			Frame->BlockManager->CurrentInterior->EndDrag();
			SetState(State::Normal);

			Renderer& r = *Frame->renderer;
			r.Dirty = true;
			r.Render();
			return;
		}
		if (state == State::Marking) {
			SetState(State::Normal);

			Renderer& r = *Frame->renderer;
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			b.ClearMarked();
			r.Dirty = true;
			r.Render();
			return;
		}
		if (state == State::AreaFirstPoint) {
			SetState(State::Normal);
			AreaFirstPoint = {};
			AreaSecondPoint = {};
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			
			b.ClearMarked();

			Renderer& r = *Frame->renderer;
			r.GetAreaSelectVerts().Clear();
			r.Dirty = true;
			r.Render();
		}
		break;
	case WXK_RETURN:
		if (state == State::Normal) {
			Renderer& r = *Frame->renderer;
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
			if (h != 0) {
				b.SetMarked(h, true);
				r.Dirty = true;
				r.Render();

				SetState(State::Marking);
				return;
			}

			SetState(State::PlacingLine);
			Frame->BlockManager->CurrentInterior->StartDrag(MouseIndex);
			return;
		}
		if (state == State::PlacingLine) {
			if (Frame->BlockManager->CurrentInterior->AddDrag(MouseIndex)) {
				Frame->BlockManager->CurrentInterior->EndDrag();
				SetState(State::Normal);
			}
			Renderer& r = *Frame->renderer;
			r.Dirty = true;
			r.Render();
			return;
		}
		if (state == State::Marking) {
			Renderer& r = *Frame->renderer;
			VisualBlockInterior& b = *Frame->BlockManager->CurrentInterior;
			unsigned int h = r.GetHighlited(Keyboarddata.MousePosition);
			if (h != 0) {
				b.SetMarked(h, !b.GertMarked(h));
			}
			else {
				b.ClearMarked();
			}
			r.Dirty = true;
			r.Render();
			return;
		}
		break;
#ifdef UseCollisionGrid
	case 'I':
		VisualBlockInterior::BoxSize++;
		//Frame->BlockManager->CurrentInterior->CollisionMap.clear();
		break;
	case 'K':
		VisualBlockInterior::BoxSize--;
		//Frame->BlockManager->CurrentInterior->CollisionMap.clear();
		break;
#endif
		/*case WXK_SPACE:
			BottomupMenu.Show(!BottomupMenu.IsShown());
			BottomupMenu.UnselectAll();
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
			break;
		case WXK_ESCAPE:
			BottomupMenu.Show(false);
			BottomupMenu.UnselectAll();
			App->ContextBinder->BindContext(App->GlContext.get());
			BottomupMenu.Render();
			SetState(Normal);
			break;*/
			/*case '1':
				SelectNthButton(0);
				break;
			case '2':
				SelectNthButton(1);
				break;
			case '3':
				SelectNthButton(2);
				break;
			case '4':
				SelectNthButton(3);
				break;
			case '5':
				SelectNthButton(4);
				break;
			case '6':
				SelectNthButton(5);
				break;
			case '7':
				SelectNthButton(6);
				break;
			case '8':
				SelectNthButton(7);
				break;
			case '9':
				SelectNthButton(8);
				break;*/
				//case '0':
				/*case WXK_BACK:
				case WXK_DELETE:
					SelectNthButton(9);
					break;*/
					//case 'Q':
						//PickNextPlaceBuilding();
						//board->
						//break;
					//case 'R':Rotate Next Place Building;
						//RotateNextPlaceBuilding();
						//break;
					//case 'F':
						//PlaceNextPlaceBuilding();
						//DeleteUnderCursor();
						//break;
					//case 'T':
						//FlipNextPlaceBuilding();
	}
}