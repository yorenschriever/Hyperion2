
export class MidiBridge {
    socket = null;
    midi = null;
    devices=[]
    deviceEnumerator=0;
    error=false;

    constructor() {
      navigator.permissions.query({ name: "midi"}).then((result) => {
        console.log('permission result', result.state, result.status)
        if (result.state === "granted") {
          this.initialize();
        } else if (result.state === "prompt") {
          this.error = true;
          // console.log('asking permission')
          // result.onchange = () => {
          //   console.log('got permission')
          //   this.initialize();
          // };
          console.error("No midi permission")
          alert("No midi permission. Please allow midi access in your browser settings and refresh the page.")
        }
      });
    }

    initialize(){

      const wsProtocol = window.location.protocol === "https:" ? "wss:" : "ws:";
      const path = "/ws/midiBridge"
      this.socket = new WebSocket(`${wsProtocol}//${location.host}${path}`);

      this.socket.onmessage = wsmsg => {
        const parsed = JSON.parse(wsmsg.data)
        this.onWsMessage(parsed.deviceId, parsed.message)
      }
      this.socket.onclose = (e) => {
        console.log(`MidiBridge Socket is closed. `);
      };
      this.socket.onerror = (err) => {
        console.error('MidiBridge Socket encountered error: ', err, 'Closing socket');
        this.error = true;
        this.socket.close();
      };
      this.socket.onopen = () => {
        console.log(`MidiBridge socket is opened on path ${path}`);
        navigator.requestMIDIAccess().then(this.onMIDISuccess.bind(this), this.onMIDIFailure.bind(this));
      }
    }

    onMIDISuccess(midiAccess) {
      console.log("MidiBridge MIDI ready!", midiAccess.inputs);
      this.midi = midiAccess;

      midiAccess.onstatechange = (event) => {
        console.log("MidiBridge state change")
        console.log(event.port.name, event.port.state, event.port);
        this.createPairs();
      };

      this.createPairs();
    }

    onMIDIMessage(event) {
      console.log('midi message')
      const device = this.devices.find(d=> d.in.id == event.currentTarget.id)
      if (!device) return;

      const message = [...event.data].map(c => c.toString(16).padStart(2, '0')).join('')

      this.socket.send(JSON.stringify({
          type: "message",
          deviceId: device.deviceId,
          message
        }))
    }

    onMIDIFailure(msg) {
      this.error = true;
      console.error(`Failed to get MIDI access - ${msg}`);
    }

    createPairs()
    {
      this.markAllDevicesForDeletion()

      let in_list = [...this.midi.inputs].map(i=>i[1])
      let out_list = [...this.midi.outputs].map(i=>i[1])
      for(const in_entry of in_list)
      {
        //TODO aliases, from ws?
        const match = out_list.find(out_entry => out_entry.name === in_entry.name)
        if (match)
        {
          this.createEntry(in_entry, match)
          out_list = out_list.filter(i=>i.id != match.id)
        } else {
          this.createEntry(in_entry, null)
        }
      }

      for(const out_entry of out_list)
      {
        this.createEntry(null, out_entry)
      }

      this.deleteMarkedDevices()
    }

    createEntry(in_device, out_device)
    {
      const compareWithNull = (a,b) => (!a && !b) || a==b; 
      const match = this.devices.find(d=> 
        compareWithNull(in_device?.id, d.in?.id) &&
        compareWithNull(out_device?.id, d.out?.id)
      )
      if (match)
      {
        match.toBeDeleted = false;
        return
      }

      const newDevice = {
        in: in_device,
        out: out_device,
        toBeDeleted: false,
        deviceId: ++this.deviceEnumerator,
        name: in_device?.name || out_device?.name
      }

      if (in_device)
      {
        in_device.onmidimessage = this.onMIDIMessage.bind(this)
      } 

      this.devices.push(newDevice);
      this.socket.send(JSON.stringify({
          type: "connected",
          deviceId: newDevice.deviceId,
          name: newDevice.name
        }))
    }

    markAllDevicesForDeletion()
    {
      this.devices.forEach(d=>d.toBeDeleted = true);
    }

    deleteMarkedDevices()
    {
      this.devices.forEach(device => {
        if (device.toBeDeleted){

          if (device.in) device.in.onmidimessage = null;
          device.in?.close();
          device.out?.close();

          this.socket.send(JSON.stringify({
            type: "disconnected",
            deviceId: device.deviceId
          }))
        }
      });

      this.devices = this.devices.filter(d=>d.toBeDeleted == false);
    }

    onWsMessage(deviceId, message)
    {
      const match = this.devices.find(d => d.deviceId == deviceId);
      if (!match) {
        return;
      }
      const messageBytes = this.hexToBytes(message);
      match.out.send(messageBytes);
    }

    hexToBytes(hex) {
      let bytes = [];
      for (let c = 0; c < hex.length; c += 2)
          bytes.push(parseInt(hex.substr(c, 2), 16));
      return bytes;
    }

    destroy()
    {
      if (this.midi) this.midi.onstatechange = null;
      this.markAllDevicesForDeletion()
      this.deleteMarkedDevices()
      this.socket?.close();
      delete this.socket;
      delete this.midi;
    }

  };
