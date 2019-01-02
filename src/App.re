/* Record
   Les records sont comme des objets JavaScript mais sont
       plus léger
       immuable par défaut
       fixes au niveau des noms de champ et des types
       très rapide
       typés de façon un peu plus rigide */
type state = {
  input: string,
  isLoading: bool,
};

/* Variant!
   La majorité des structures de données dans la plupart des langages concernent «ceci et cela». Un variant nous permet d'exprimer «ceci ou cela». */

type action =
  | UpdateInput(string)
  | Search;

/*
 il y a deux types de composants ReasonReact.statelessComponent("App") et ReasonReact.reducerComponent("App")
 declarer le composant apres les actions et le state
 */
let component = ReasonReact.reducerComponent("App");

/*
 Pour avoir un stateful component :
 1 - il faut rajouter un initialState sous forme d'une function
 qui retourne le record avec les valeurs par default
 2 - il faut rajouter le reducer, un peu comme redux en utilisant le pattern-matching
 3 - modification du render, remplacer _self par self nb : c'est comme le this
 */
let make = _children => {
  ...component,
  initialState: () => {input: "", isLoading: false},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(newInput) =>
      ReasonReact.Update({...state, input: newInput})
    | Search =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isLoading: true},
        (
          self => {
            let value = self.state.input;
            /* This function needs to return a "unit" type, wo we'll insert it here */
            ();
          }
        ),
      )
    },
  render: self =>
    <div>
      <form
        onSubmit={
          ev => {
            ReactEvent.Form.preventDefault(ev);
            self.send(Search);
          }
        }>
        <label htmlFor="search"> {ReasonReact.string("Search")} </label>
        <input
          id="search"
          name="search"
          value={self.state.input}
          onChange={
            ev => {
              /** event->ReactEvent.Form.target##value */
              let value = ReactEvent.Form.target(ev)##value;
              self.send(UpdateInput(value));
            }
          }
        />
        <button type_="submit">
          {ReasonReact.string("Submit Search")}
        </button>
      </form>
      <div>
        {
          self.state.isLoading ?
            ReasonReact.string("Loading...") : ReasonReact.null
        }
      </div>
    </div>,
};