
# Table of Contents

1.  [Introduction](#org1117906)
2.  [Outline](#orgde85c1e)
3.  [ESP32](#org716d0ab)
4.  [Matrix scanning](#org5357480)
    1.  [Ghosting](#org5ae6392)
    2.  [Key debouncing](#org9d6fe18)
    3.  [Changes](#org08e8dd8)
5.  [ESP-Now](#org13bedc9)
    1.  [Mesh interface class](#org281bd2a)
    2.  [Changes](#org4c65541)
6.  [Modifier keys](#orgcbf3146)
    1.  [Changes](#org2e4ae43)
7.  [Bluetooth](#orgebca092)
    1.  [Changes](#orgc331644)
8.  [Keyboard layers](#orgce4f577)
    1.  [Changes](#org721c4d9)
9.  [OLED Display](#orgb2d08dd)
    1.  [Changes](#orgbea234a)
10. [Battery control](#org1d8973c)
    1.  [Changes](#orgdf3dd0c)
11. [Backlog and weird behavior notes](#org7fb8bf7)


<a id="org1117906"></a>

# Introduction

<span class="timestamp-wrapper"><span class="timestamp">&lt;2021-06-21 Mon&gt;</span></span>
Mechanical keyboards  are somewhat  of a dated  concept that
has  gathered some  speed in  more recent  years. Back  when
computers were  first coming  out for the  public, keyboards
had  mechanical connections  that would  allow a  current to
activate a  switch. Then as  economics got wind,  somehow we
got used  to membrane  keyboards; virtually every  office in
the  world   has  these  membrane  keyboards.   Compared  to
mechanical keyboards, membrane  keyboards feel more &ldquo;mushy&rdquo;.
In contrast,  mechanical keyboards  may have  different feel
based  on  the  springs  or  whether the  key  switch  has  a
noticeable  &ldquo;bump&rdquo;.   They  can   be  clicky  or   not;  the
possibility are wild now.

A  few  years  ago  I started  following  the  subreddit  on
mechanical keyboards.  Back then,  the community  had little
options and cherry still had patents on the switches. Cherry
MX keys were virtually found in every commercial keyboard on
the market. Now,  the landscape has changed quite  a bit and
more and more different types of switches are available.

Why do  I prefer mechanical keyboards?  Mechanical keyboards
give a &ldquo;joy&rdquo; to typing.  Membrane keyboards are fine to type
on, but  they aren&rsquo;t fun.  Generally my fingers  get &ldquo;tired&rdquo;
after typing  on membrane keyboards  as the keys feel  a bit
mushy, i.e.  you get no relief  when pushing down a  key and
the key press does not feel crisp.

After  following  / r / mechanicalkeyboards   for  a  while,  I
decided to  *build* my  own keyboard. Within  the community,
there is a wide variety  of switches and shapes of keyboards
available. The one I wanted, an orthonormal keyboard, wasn&rsquo;t
commercially available. Plus I liked the idea of building my
own keyboard.  In my work,  the keyboard is my  primary tool
and why not  key a custom version of it  for myself. I ended
up  building 2  handwired  versions of  65  percent with  85
switches  in  a  grid  layout.   In  addition,  I  modded  a
MagSafe-inspired cable to it which allowed it to be nice and
portable. After  every build I told  myself : &ldquo; This  is the
last one I&rsquo;ll build&rdquo;.

Enter  this blog,  where I  again tell  myself &ldquo;this  is the
last  one I  build!&rdquo;. What  changed? Since  I learned  about
split-style keyboard,  I always wanted one.  The keyboards I
had  made   so  far  weren&rsquo;t.  Split   would  be  completely
ergonomical,  but  unfortunately,  none (that  I  know  of)
exists that are both (a) wireless and (b) split. As always
I aimed to high and wanted to emulate the many features that
QMK implements. Most importantly, I  wanted to get back into
writing more low  level languages like c++, and  I take this
project as  a nice  opportunity to get  into c++  again, and
work with micro-controllers.

This post  will serve as  my log for building  the keyboard.
The post will updated as I work on it.

Core feature targets

-   Split <span class="underline">wireless</span> keyboard
-   Hot swappable key sockets
-   Portable, not a full keyboard
-   Battery control
-   OLED display
-   Rotary encoders


<a id="orgde85c1e"></a>

# Outline

The keyboard is split; it has two halves. The right and left
half will have  most of the same  &ldquo;base&rdquo; functionality. Most
importantly, each  half needs to  scan the matrix  to obtain
which keys are being pressed. One of the halves will act as a
server, the other will act as a client. The server will need
the following capabilities

**Server abilities**

-   Read matrix
-   Setup a bluetooth connection
    -   HID Device
    -   Mouse emulation
-   Setup connection with client
    -   Merge keys pressed and send to bluetooth controller
-   Control LEDs on both client and server

**Client abilities**

-   Read matrix
-   Find server and send pressed keys to server

Due to the heavier load of  the server, I prefer to make the
role of  who is server and  who is client dynamic.  That is,
with some heuristic (for example  deep sleep), the roles may
switch to prolong batter life of both units.

To give a course overview consider the following picture:

![img](./figures/overview.png)


<a id="org716d0ab"></a>

# ESP32

<span class="timestamp-wrapper"><span class="timestamp">&lt;2021-07-12 Mon&gt;</span></span>-
I  opted for  a micro-controller  as this  would allow  me to
prototype without  worrying about my electronic  skills. The
controller needed  to have  battery control,  bluetooth, and
preferable an  energy efficient screen;  I ended up  with an
esp32.

The esp32 is a hybrid chip that has both Wi-Fi and bluetooth
capabilities. The esp32 consists  of different versions that
varies in (mainly) in the number of pins, battery connector,
and  or screen.  The version  I ended  with (LORA-V2)  had a
battery connector and a tiny OLED screen.

The  ecosystem  of  ESP32   is  well-developed  albeit  less
convenient  than  its  arduino  counterparts.  Luckily,  the
opensource community has taken it upon themselves to provide
lots  of  arduino bindings  to  the  libraries by  espressif
(manufacturer of esp32).

Especially important  (as it turned  out later) is  that the
esp32 has the capabilities of using both Wi-Fi and bluetooth
low  energy simultanaously.  In  addition, through  ESP-NOW,
different  eps32  modules can  form  a  mesh, which  I  will
harness to do server-client communication.

![img](./figures/pinout.jpg "Pin-out ESP32 LORA-V2")


<a id="org5357480"></a>

# Matrix scanning

<span class="timestamp-wrapper"><span class="timestamp">&lt;2021-07-12 Mon&gt;</span></span>
A keyboard  matrix scanning circuit  is used to  enhance the
number  of keys,  while keeping  the number  of pins  low. A
micro-controller  uses general  pin  input/output (GPIO)  to
register currents.  If a singular  key switch is wired  to a
single pin, 96 pins would be needed for a 104 sized keyboard
(full-size). This would be unpractical.

As an alternative  one could apply matrix  scanning. In this
method,  the keys  are wired  as  a grid  where each  column
connects to each  row effectively forming a  &ldquo;switch&rdquo;. For a
total for 100 keys, one would need 10x10 grid. The grid acts
as a force multiplier for the number of switches. Instead of
needing 100  separate keys,  we merely need  10 rows  and 10
columns (20 pins) to wire our 100 switch keyboard.

The  matrix is  repeatedly  scanned to  determine  if a  row
column form  an open circuit.  That is,  if a key  switch is
pressed down, current  can flow between the  row and column.
The scanning occurs at a high scan rate, making it seemingly
instantaneous.


<a id="org5ae6392"></a>

## Ghosting

Matrix  scanning  forms  an excellent  idea  to  efficiently
represent our electronic  switches. However, merely scanning
does  not  correctly records  all  key  presses. Under  some
conditions, a matrix  can record ghost keys,  i.e. keys that
are  registered  but not  pressed.  This  process is  called
ghosting.

Ghosting  occurs  when  current   can  freely  flow  between
separate  rows or  columns due  to another  row/column being
open.  For example  consider a  simple two  row, two  column
keyboard. This board can support 4 keys. When two keys along
the diagonal  are pressed,  we register  4 keys(!).  This is
obviously wrong and  needs to be corrected.  The most common
approach is to put a diode  right after the switch either on
the columns or rows,  which prevents current from traversing
and causing ghosting.

![img](./figures/ghosting.png "Ghosting example. Ghosting occurs when current can flow freely across columns and rows. (Left) one key is pressed down bottom left. (Middle) A key across from the first is activated which causes ghosting (right); current flows from the second row, first column to the second row, second column etc.")


<a id="org9d6fe18"></a>

## Key debouncing

Key  debounce is  a mechanism  to filter  out erroneous  key
activity.  When  two metal  plates  come  into contact,  the
signal does not form a clean  square wave. In order to clean
up  this  signal, key  debouncing  is  used to  reflect  the
&ldquo;press&rdquo; of key switch.


<a id="org08e8dd8"></a>

## Changes

-   [X] Added matrix class
    -   [X] added matrix scan
    -   [X] added key debounce
    -   [X] added (whole) matrix debounce
        -   [X] filters out erroneous key presses


<a id="org13bedc9"></a>

# ESP-Now

<span class="timestamp-wrapper"><span class="timestamp">&lt;2021-06-19 Sat&gt;</span></span>
The two  halves need to  communicate to eachother.  There is
only one half that is connected through bluetooth to another
device. We call  this the server, and the  other the client.
Keys pressed on  the client needs to be  communicated to the
server  which   processes  the  keys,  and   sends  it  over
bluetooth. Luckily,  ESP-now offers a mesh  interface we can
utilize for  this purpose.  This is easier  to setup  than a
bluetooth mesh interface and should be relatively secure for
foreign attackers. From the website we read:

> ESP-NOW is yet another protocol developed by Espressif, which enables multiple devices to communicate with one another without using Wi-Fi. The protocol is similar to the low-power 2.4GHz wireless connectivity that is often deployed in wireless mouses. So, the pairing between devices is needed prior to their communication. After the pairing is done, the connection is secure and peer-to-peer, with no handshake being required.


<a id="org281bd2a"></a>

## Mesh interface class

The mesh class is responsible for:

-   Setup / deinit the ESP-now connection
-   Holding a buffer that is sent over the ESP-now connection.
    The buffer holds information that needs to be communicated
    between each  halves.

At  the moment  of writing,  the mesh  class holds  a static
buffer   which  holds   \`keyswitch<sub>t</sub>\`.  These   are  structs
containing when the last time  the pins were read as active.
In addition, it contains information  on the source and sinc
pins, and column and row indices. These last two are used to
index into the final keymap on the server side. This way, no
actual key information is send, but the server reads the key
from the  col and row,  then they are combined.  This solves
the issue of sending ascii shifted codes or media keys.


<a id="org4c65541"></a>

## Changes

-   [X] Implemented mesh interface class
-   [X] Added server capabilities to join the keys from both half and communicate through bluetooth


<a id="orgcbf3146"></a>

# Modifier keys

<span class="timestamp-wrapper"><span class="timestamp">&lt;2021-07-26 Mon&gt;  </span></span>  My  initial implementation  measures  the
onset of  keys. That is,  debounce worked by  measuring when
the &ldquo;square  wave&rdquo; of the  key was pressed. This  allows for
fast  and  accurate  detection  detecting  key  press  down.
Initially my intentions was to merge the other keys together
such that multiple keys are registered at the same time. For
example, the shift key needs to register two keys at minimum
to shift the ascii code around for let&rsquo;s say \`a\` to \`A\`.

Consequently, I need to both  register the key press down as
well as the  key release; I modified  the debounce mechanism
to also detect the key release.


<a id="org2e4ae43"></a>

## Changes

-   [X] Change key detection. Register key press and key release
-   [X] Mesh buffer management is moved out of the keyboard class.
-   [X] Fixed wrong indexing in reading the active keys on the server.


<a id="orgebca092"></a>

# Bluetooth

Bluetooth  is  rather  complicated. The  Bluetooth  Keyboard
class takes  care of  most of the  heavy lifting.  Key codes
have an associated  ascii code, these are put  into an ascii
code map. Note that the over bluetooth (for whatever reason)
these keycodes are remapped to different numbers.


<a id="orgc331644"></a>

## Changes

-   [ ] Expand  this section with info  on characteristics and
    services.
-   [X] Figure out  how  the key  codes  are organized  The
    symbols are organized in a 128 ascii keymap containing the
    hex codes to  a symbol. Hex codes can be  send directly in
    addition to  normal strings  over bluetooth.  The modifier
    keys  in  combination with  some  media  control keys  are
    defined   in  &ldquo;BleKeyboard.h&rdquo;,   the  ascii   map  is   in
    &ldquo;BleKeyboard.cpp&rdquo;.  I  have  written a  short  wrapper  in
    &ldquo;key<sub>defintions.hpp</sub>&rdquo;.
-   [X] Add functions for  interfacing with bluetooth  to the
    keyboard class
    -   [X] Pressing down keys
    -   [X] Releasing keys
-   [ ] Convert config class  to static class
-   [-] Write layer keymap for keyboard
    -   [X] Wrote qwerty base layer
    -   [ ] Add fixed array check to the layers (add to constant
        config class steps)


<a id="orgce4f577"></a>

# Keyboard layers

A layer  is implemented as a  2D vector for the  moment, but
will  likely change  in finalizing  the keyboard.  An active
layer is set as a pointer  to the current active layer. Each
keyswitch has  information on  where in  the grid  they fit;
keys are read by using these indices in the 2D vector. I did
consider an  unordered<sub>map</sub> use the keyswitch  directly as an
indicator. This could then be combined with pointers to make
a  layer dynamic,  i.e.  instead of  having  the concept  of
layers, each key has a different layer that can be accessed.
This adds  some complexity and  I decided against  this. The
main reason is that the client side would then need to store
information  on what  each keyswitch  points to.  This would
increase communication between each  halves if modifiers are
used for example. I am  afraid that this added communication
is not as trivially solved,  i.e. one needs to send modifier
key across ESP-NOW and then shift all affected keys and when
activated send this information back. The ESP-NOW channel is
not designed for high information throughput.

The keyboard is  not going to be full size.  That is, purely
based on the number of keys,  this keyboard will not be able
to have a 1-to-1 mapping  from symbol to keyswitch. Luckily,
we  can  greatly  increase  the number  of  symbols  on  the
keyboard by hosting the missing symbols on different layers.
This means we have to implement a feature that allows one to
switch  between   different  layers.  For  example   we  may
implement a layer up and layer  down key, or allow to switch
directly between different  layers. In QMK is  worked out by
an \`enum struct\`.  Layers are stacked on top  of each other.
This has the  added feature of allowing  a &ldquo;transparent&rdquo; key
to access on a layer below. I wish to emulate this feature.

I currently host my key layer  as a 2d vector. In finalizing
my build this  may change to a fixed array  size. As vectors
can be arbitrary sized, I need to add a check to the vectors
to not allow uses to  define oddly sized arrays (which would
lead to  seg faults).  This will be  added to  the finalized
checks.

In  QMK layers  are \`enum\` type, which  means the  layers are
number  and tracked  through an  int. Here,  I will  have an
\`active<sub>layer</sub>\`  which points  to the  \`layer<sub>t</sub>\` hosting  the
current  active keys.  With transparent  keys I  can imagine
that this approach will not work.


<a id="org721c4d9"></a>

## Changes

-   [ ] Implement key layers
    -   [ ] KC<sub>TRANS</sub> accesses key below the current layer
        -   [ ] This effect may  stack until a non-transparent key
            is found
        -   [ ] Layer switch keys
            -   [ ] Up and down
        -   [ ] Hold  layer switch key: similar  to modifier keys,
            these  keys  temporarily   shift  the  key layer  while
            holding down this key.


<a id="orgb2d08dd"></a>

# OLED Display


<a id="orgbea234a"></a>

## Changes

-   [ ] Start creating interface for display management
-   [ ] Find interesting functions to put on the screen
    -   [ ] WiFi notifications?
    -   [ ] Keyboard status info
        -   [ ] Keyboard layer info
        -   [ ] Battery level info


<a id="org1d8973c"></a>

# Battery control


<a id="orgdf3dd0c"></a>

## Changes

-   [ ] Implement battery control


<a id="org7fb8bf7"></a>

# Backlog and weird behavior notes

-   Pressing  down  a  key  repeatedly and  then  another  key
    afterwards, stops sending the  initially pressed down key.
    For example holding down \`a\` and then pressing any other
    key (including modifies) stops sending \`a\`.
-   Figure out bug  where \`-\` is sent  repeatedly. This occurs
    especially when sending \`a\` key.  I think it is related to
    the ascii code for for \`a\` and \`-\`.
-   Figure out bug  where &rsquo;up arrow&rsquo; is  sent repeatedly. This
    occurs  when the  keyboard is  connected to  bluetooth. No
    keys are send on my part.

