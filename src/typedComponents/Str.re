let component = ReasonReact.statelessComponent("Str");

let make = (~string, _children) => {
  ...component,
  render: _self => ReasonReact.string(string),
};