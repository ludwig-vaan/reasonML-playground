let component = ReasonReact.statelessComponent("Card");

module Styles = {
  open Css;
  let card =
    style([
      border(px(1), `solid, hex("898989")),
      borderRadius(px(4)),
      padding(rem(1.0)),
    ]);
};
/* /!\ className=Styles.... */
/* need to use <div className=Styles.card /> with open Css module */

/* classic way with inline style:
   Inside of src/Card.re
   module Styles = {
     let card =
       ReactDOMRe.Style.make(
         ~border="2px solid #898989",
         ~borderRadius="4px",
         ~padding="1rem",
         (),
       );
   };

   Later, in our `render` JSX
   /!\ style=Styles....
   <div style=Styles.card> */

let make = (~name, ~description, ~href, _children) => {
  ...component,
  render: _self =>
    <div className=Styles.card>
      <h3>
        <a href target="_blank" rel="noopener noreferrer">
          {ReasonReact.string(name)}
        </a>
      </h3>
      <p> <Str string=description /> </p>
    </div>,
};