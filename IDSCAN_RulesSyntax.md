# CANBus IDS Rules Syntax Guide

The CANBus IDS rules syntax is inspired by Snort/Suricata rules syntax, with an adaptation to the specifications of CANBus frames syntax.

## Global Rule Syntax

### Classic Snort/Suricata rule syntax

The original rule syntax for Snort and Suricata is composed of three fields :

| Field | Example | Description |
| --- | ----------- | --------- |
| @action  | *alert* | The action to perform |
| @header  | *tcp $EXTERNAL_NET 80 -> $HOME_NET any* | Protocol, addresses and ports the rule applies to |
| @options | *(...)* | Message associated with rule, and optional criterias for the rule |

Here is an example of such a frame, with the corresponding values from the previous table :

```javascript
alert tcp $EXTERNAL_NET 80 -> $HOME_NET any
(
    msg:"Attack attempt!";
    flow:to_client,established;
    file_data;
    content:"1337 hackz 1337",fast_pattern,nocase;
    service:http;
    sid:1;
)
```

### CANBus IDS rule syntax

The syntax of a CANBus IDS rule is very similar, it is composed of 3 fields as well:

| Field | Example | Description |
| --- | ----------- | --------- |
| @action  | *alert* | The action to perform |
| @header  | *C 0x100 D ->* | IDs and direction of the flow the rule applies to. |
| @options | *(...)* | Message associated with rule, and optional criterias for the rule |

Here is an example of such a frame, with the corresponding values from the previous table :

```javascript
alert C 0x100 D ->
(
    msg:"Attack attempt!";
    content:"1337 hackz 1337",fast_pattern,nocase;
    sid:1;
)
```

A global skeleton of a CANBus IDS rule would be :
 ```javascript
 action ID_type ID1 ID2(optional) Frame_type Direction_symbol
 (
    // Any allowed options
 )
 ```

## Possible values for the different fields

For each field used to describe the rule, there are some constraints so that they can be understood by the parser in the IDS itself.

### Action field

The action field has X different possible options :
<ol>
    <li> <strong>alert</strong> : generate an alert on the current packet </li>
    <li> <strong>block</strong> : block the current packet and blacklists the ID/Node </li>
    <li> <strong>drop</strong> : drop the current packet </li>
    <li> <strong>log</strong> : log the current packet </li>
    <li> <strong>pass</strong> : mark the current packet as passed </li>
    <li> <strong>rewrite</strong> : enables overwrite packet contents based on a "replace" option in the rules </li>
</ol>

### Header field

The header field only contains the IDs of the frames this rule should apply to. It contains three to four fields :

| Field | Possible Values | Description |
| ----- | ------- | ----------- |
| ID_Format | C or E | C is for classic 11-bit identifiers, E is for extended 29 bits identifiers |
| ID1 | 0x0 to 0x7FF or any | Specific 11bit id value, or any 11bit value. |
| ID2 (optional) | 0x0 to 0x4000 or any | Specific 18bit extension id value, if used, or any 18bit value |
| Frame_type | D or R | D is for data frames, R is for remote request frames |

<strong><em>TODO : Ajouter la possibilite de mettre une range d'ID, ou une liste d'ID</strong></em>

The last component of the header field is the direction symbol, which has 3 possible values:

| Value | Description |
| ----- | ----------- |
| -> | In the case the frame is sent to CANBus |
| <- | In the case the frame is received from the CANBus |
| <> | In the case the rule has to be applied to both directions | 

Since all messages are broadcasted on CANBus, there are no destinations if the frame is sent ( -> ) and no source when the frame is received ( <- ).

### Options field

There are many options that could be used to further analyze a CANBus frame, in our case we have decided to implement a few of them that are described in this section.

If necessary, new options could easily be implemented to customize the rules to special use cases.

<strong><em>TODO :</strong></em>

Check data length according to id.

Check data value according to ID and acceptable values (>0, or between 0x20 and 0xFF for example).

In the case of Remote Request, allowed to do the request? Is it well formed? Have to study this more

Add a period value maybe, that would help detect spoofing for example, or even DOS

Sequencement