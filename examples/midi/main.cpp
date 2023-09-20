#include "colours.h"
#include "distribution/inputs/controlHubInput.hpp"
#include "distribution/outputs/monitorOutput.hpp"
#include "distribution/pipes/convertPipe.hpp"
#include "generation/patterns/mappedPatterns.h"
#include "hyperion.hpp"
#include "ledsterMap.hpp"
/*

This example shows how to manually attach a midi controller to a midi device.

A midi device is an instance representing the hardware that is sending/receiving the midi messages on the hyperion side.
A midi controller is an instance that read the midi data from the midi device and translates it to control instructions
for the control hub. eg: "note 89 on" means "button in column 2 slot 4 is pressed" or "controller 44 change" means
"column 4 dim changes".

It is important that the correct midi controller instance is created when a device connects, otherwise the incoming data
makes no sense. Normally, On devices with usb midi, the name of the device is used pick which class to instantiate.
The MidiControllerFactory has logic to determine that.

On DIN midi connections, however, we dont have an instance name. The device will be named UNKNOWN_DIN_MIDI_DEVICE,
and you manually have to tell which device you will connect. You do this by subclassing the MidiControllerFactory,
and setting that instance in the hyperion object.

See the example below
*/

// Create the subclass
class CustomMidiControllerFactory : public MidiControllerFactory
{
public:
  // override the method that determines which controller to pick for a given device
  virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub) override
  {
    // if we are dealing with a din midi device, pick a specific controller. in this case the ApcMiniController
    if (name.compare(UNKNOWN_DIN_MIDI_DEVICE) == 0)
      return std::unique_ptr<ApcMiniController>(new ApcMiniController(hub, device));

    // otherwise, return the value that the original MidiControllerFactory would have returned
    return MidiControllerFactory::create(device, name, hub);
  }
};

int main()
{
  auto hyp = new Hyperion();
  // tell hyperion to use our custom midiControllerFactory
  hyp->midiControllerFactory = new CustomMidiControllerFactory();

  // the rest of the code is a simple controller, so you can see that you device actually connects to something.
  auto pipe = new ConvertPipe<RGBA, RGB>(
      new ControlHubInput<RGBA>(
          ledsterMap.size(),
          &hyp->hub,
          0,
          {
              new Mapped::ConcentricWavePattern<SinFast>(&ledsterMap),
              new Mapped::HorizontalGradientPattern(&ledsterMap),
          }),
      new MonitorOutput(&hyp->webServer, &ledsterMap));

  hyp->addPipe(pipe);

  hyp->start();

  while (1)
    Thread::sleep(1000);
}
