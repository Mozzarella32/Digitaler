#include "pch.hpp"
#include "RenderTextUtility.hpp"
//
//TextSDFManager::TextSDFManager() {
//	StartWorker();
//}
//
//void TextSDFManager::StartWorker() {
//	Terminate = false;
//	worker = std::thread([this]() {Loop(); });
//}
//
//void TextSDFManager::StopWorker() {
//	Terminate = true;
//	cv.notify_all();
//	worker.join();
//}
//
//TextSDFManager::~TextSDFManager() {
//	StopWorker();
//}
//
//void TextSDFManager::AddToQueue(const std::string& string) {
//	std::unique_lock ul(QueuesMutex);
//	if (Queue.contains(string)) return;
//	Queue.insert(string);
//	ToBeRendered.push(string);
//}
//
//wxImage TextSDFManager::RenderTextToImage(const wxString& text, const wxFont& font, int width, int height) {
//	wxBitmap bitmap(width, height, 24); // RGB-Bitmap
//	wxMemoryDC memoryDC;
//	memoryDC.SelectObject(bitmap);
//
//	// Hintergrund füllen
//	memoryDC.SetBrush(*wxWHITE_BRUSH);
//	memoryDC.Clear();
//
//	// Text zeichnen
//	memoryDC.SetFont(font);
//	memoryDC.SetTextForeground(*wxBLACK);
//	memoryDC.DrawText(text, 10, 10);
//
//	memoryDC.SelectObject(wxNullBitmap); // Verbindung lösen
//
//	// In wxImage konvertieren
//	return bitmap.ConvertToImage();
//}
//
//void TextSDFManager::ConvertImageToShape(const wxImage& image, msdfgen::Shape& shape) {
//	int width = image.GetWidth();
//	int height = image.GetHeight();
//
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			if (image.GetRed(x, y) == 0) { // Schwarz = Teil des Textes
//				shape.addContour().addEdge(msdfgen::EdgeHolder(new msdfgen::LinearSegment(
//					msdfgen::Point2(x, y),
//					msdfgen::Point2(x + 1, y))));
//			}
//		}
//	}
//	shape.normalize();
//}
//
//std::vector<uint8_t> TextSDFManager::GenerateMSDFTexture(msdfgen::Shape& shape, int width, int height) {
//	std::vector<uint8_t> outputTexture = std::vector<uint8_t>(width * height * 3);
//
//	msdfgen::Bitmap<float, 3> msdfBitmap(width, height);
//
//	// Edge Coloring vorbereiten (Shape wird modifiziert)
//	msdfgen::edgeColoringSimple(shape, 3.0); // 3.0 ist die durchschnittliche Segmentlänge
//
//	// MSDF generieren (mit Standard-Transformation)
//	msdfgen::generateMSDF(msdfBitmap, shape, msdfgen::SDFTransformation());
//
//	// Ausgabe-Array vorbereiten (RGB-Werte in 8 Bit pro Kanal)
//	outputTexture.resize(width * height * 3);
//
//	for (int y = 0; y < height; ++y) {
//		for (int x = 0; x < width; ++x) {
//			auto pixel = msdfBitmap(x, y);
//			outputTexture[(y * width + x) * 3 + 0] = static_cast<uint8_t>(std::clamp(pixel[0] * 255.0f, 0.0f, 255.0f));
//			outputTexture[(y * width + x) * 3 + 1] = static_cast<uint8_t>(std::clamp(pixel[1] * 255.0f, 0.0f, 255.0f));
//			outputTexture[(y * width + x) * 3 + 2] = static_cast<uint8_t>(std::clamp(pixel[2] * 255.0f, 0.0f, 255.0f));
//		}
//	}
//
//	return outputTexture;
//}
//
//std::unique_ptr<Texture> TextSDFManager::GenerateSDFTexture(const std::vector<uint8_t>& vec, int width, int height) {
//	return std::make_unique<Texture>(width, height, (void*)vec.data(), Texture::Descriptor{
//		.Wrap_S = Texture::Descriptor::Wrap_Type::REPEAT,
//		.Wrap_T = Texture::Descriptor::Wrap_Type::REPEAT,
//		.Min_Filter = Texture::Descriptor::Filter_Type::LINEAR,
//		.Mag_Filter = Texture::Descriptor::Filter_Type::LINEAR,
//		.Internal_Format = GL_RGB8
//		});
//}
//
//void TextSDFManager::Loop() {
//	while (!Terminate) {
//		std::unique_lock lock(CVMutex);
//		cv.wait(lock, [this]() {
//			std::unique_lock ul(QueuesMutex);
//			return !ToBeProcessed.empty() || Terminate; });
//
//		if (Terminate) return;
//
//		while (!ToBeProcessed.empty()) {
//			std::unique_lock ul(QueuesMutex);
//			auto [s, image] = ToBeProcessed.front();
//			ToBeProcessed.pop();
//			ul.unlock();
//
//			msdfgen::Shape shape;
//			ConvertImageToShape(image, shape);
//
//			auto bytes = GenerateMSDFTexture(shape, image.GetWidth(), image.GetHeight());
//			ToBeMadeIntoTexture.emplace(s, bytes, image.GetWidth(), image.GetHeight());
//		}
//	}
//}
//
////Only Main Thread
//std::optional<std::reference_wrapper<Texture>> TextSDFManager::GetSDF(const std::string& string) {
//	std::unique_lock ul(QueuesMutex);
//	auto it = SDFs.find(string);
//	if (it != SDFs.end()) {
//		return *it->second.get();
//	}
//	ul.unlock();
//	AddToQueue(string);
//	return {};
//}
//
//void TextSDFManager::setFont(const wxFont& f) {
//
//	StopWorker();
//
//	SDFs.clear();
//
//	while (!ToBeMadeIntoTexture.empty()) ToBeMadeIntoTexture.pop();
//	while (!ToBeProcessed.empty()) ToBeProcessed.pop();
//	while (!ToBeRendered.empty()) ToBeRendered.pop();
//
//	font = f;
//
//	StartWorker();
//}
//
////Only Main Thread
//void TextSDFManager::ProcesOrRenderSthAtMostNTimes(int n) {
//	std::unique_lock ul(QueuesMutex);
//	for (int i = 0; i < n && (!ToBeRendered.empty() || !ToBeMadeIntoTexture.empty()); i++) {
//		if (!ToBeRendered.empty()) {
//			std::string s = ToBeRendered.front();
//			ToBeRendered.pop();
//			auto image = RenderTextToImage(s, font, 256, 256);
//			ToBeProcessed.emplace(s, std::move(image));
//			cv.notify_all();
//		}
//
//		if (!ToBeMadeIntoTexture.empty()) {
//			auto [s, data, wight, height] = ToBeMadeIntoTexture.front();
//			ToBeMadeIntoTexture.pop();
//			SDFs[s] = GenerateSDFTexture(data, wight, height);
//			Queue.erase(s);
//		}
//	}
//}
//
//TextSDFManager& TextSDFManager::Instance() {
//	static TextSDFManager instance;
//	return instance;
//}

//using namespace msdf_atlas;


bool generateAtlas(const char* fontFilename) {
	bool success = false;
	//Initialize instance of FreeType library
	//if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype()) {
	//	// Load font file
	//	if (msdfgen::FontHandle* font = msdfgen::loadFont(ft, fontFilename)) {
	//		// Storage for glyph geometry and their coordinates in the atlas
	//		std::vector<GlyphGeometry> glyphs;
	//		// FontGeometry is a helper class that loads a set of glyphs from a single font.
	//		// It can also be used to get additional font metrics, kerning information, etc.
	//		FontGeometry fontGeometry(&glyphs);
	//		// Load a set of character glyphs:
	//		// The second argument can be ignored unless you mix different font sizes in one atlas.
	//		// In the last argument, you can specify a charset other than ASCII.
	//		// To load specific glyph indices, use loadGlyphs instead.
	//		fontGeometry.loadCharset(font, 1.0, Charset::ASCII);
	//		// Apply MSDF edge coloring. See edge-coloring.h for other coloring strategies.
	//		const double maxCornerAngle = 3.0;
	//		for (GlyphGeometry& glyph : glyphs)
	//			glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
	//		// TightAtlasPacker class computes the layout of the atlas.
	//		TightAtlasPacker packer;
	//		// Set atlas parameters:
	//		// setDimensions or setDimensionsConstraint to find the best value
	//		packer.setDimensionsConstraint(DimensionsConstraint::SQUARE);
	//		// setScale for a fixed size or setMinimumScale to use the largest that fits
	//		packer.setMinimumScale(24.0);
	//		// setPixelRange or setUnitRange
	//		packer.setPixelRange(2.0);
	//		packer.setMiterLimit(1.0);
	//		// Compute atlas layout - pack glyphs
	//		packer.pack(glyphs.data(), glyphs.size());
	//		// Get final atlas dimensions
	//		int width = 0, height = 0;
	//		packer.getDimensions(width, height);
	//		// The ImmediateAtlasGenerator class facilitates the generation of the atlas bitmap.
	//		ImmediateAtlasGenerator<
	//			float, // pixel type of buffer for individual glyphs depends on generator function
	//			3, // number of atlas color channels
	//			&msdfGenerator, // function to generate bitmaps for individual glyphs
	//			BitmapAtlasStorage<byte, 3> // class that stores the atlas bitmap
	//			// For example, a custom atlas storage class that stores it in VRAM can be used.
	//		> generator(width, height);
	//		// GeneratorAttributes can be modified to change the generator's default settings.
	//		GeneratorAttributes attributes;
	//		generator.setAttributes(attributes);
	//		generator.setThreadCount(4);
	//		// Generate atlas bitmap
	//		generator.generate(glyphs.data(), glyphs.size());
	//		// The atlas bitmap can now be retrieved via atlasStorage as a BitmapConstRef.
	//		// The glyphs array (or fontGeometry) contains positioning data for typesetting text.
	//		//success = myProject::submitAtlasBitmapAndLayout(generator.atlasStorage(), glyphs);
	//		// Cleanup

	//		//msdfgen::saveBmp(generator.atlasStorage(), "atlas.bmp");

	//		msdfgen::destroyFont(font);
	//	}
	//	msdfgen::deinitializeFreetype(ft);
	//}
	return success;
}
