#include "read_test.h"

#include "podio/Frame.h"
#include "podio/ROOTLegacyReader.h"

#include <iostream>

int main(int argc, char* argv[]) {
  std::string inputFile = "example.root";
  bool assertBuildVersion = true;
  if (argc == 2) {
    inputFile = argv[1];
    assertBuildVersion = false;
  }

  auto reader = podio::ROOTLegacyReader();
  try {
    reader.openFile(inputFile);
  } catch (const std::runtime_error& e) {
    std::cout << "File (" << inputFile << ")could not be opened, aborting." << std::endl;
    return 1;
  }

  if (assertBuildVersion && reader.currentFileVersion() != podio::version::build_version) {
    std::cerr << "The podio build version could not be read back correctly. "
              << "(expected:" << podio::version::build_version << ", actual: " << reader.currentFileVersion() << ")"
              << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != 2000) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 2000 << ", actual: " << reader.getEntries("events") << ")" << std::endl;
    return 1;
  }

  for (size_t i = 0; i < reader.getEntries("events"); ++i) {
    const auto frame = podio::Frame(reader.readNextEntry("events"));
    processEvent(frame, i, reader.currentFileVersion());
  }

  // Reading specific entries
  {
    auto frame = podio::Frame(reader.readEntry("events", 4));
    processEvent(frame, 4, reader.currentFileVersion());

    auto nextFrame = podio::Frame(reader.readNextEntry("events"));
    processEvent(nextFrame, 5, reader.currentFileVersion());

    auto previousFrame = podio::Frame(reader.readEntry("events", 2));
    processEvent(previousFrame, 2, reader.currentFileVersion());
  }

  return 0;
}
