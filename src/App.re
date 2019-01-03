type repository = {
  name: string,
  description: string,
  href: string,
};

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
  results: list(repository),
};

/* Variant!
   La majorité des structures de données dans la plupart des langages concernent «ceci et cela». Un variant nous permet d'exprimer «ceci ou cela». */

type action =
  | UpdateInput(string)
  | UpdateResults(list(repository))
  | Search;
/*
 il y a deux types de composants ReasonReact.statelessComponent("App") et ReasonReact.reducerComponent("App")
 declarer le composant apres les actions et le state
 */
let component = ReasonReact.reducerComponent("App");

/**
Api
 */
module Api = {
  open Json.Decode;
  module Promise = Js.Promise;

  let decodeResults =
    field(
      "items",
      list(
        optional(json =>
          {
            name: field("name", string, json),
            description: field("description", string, json),
            href: field("html_url", string, json),
          }
        ),
      ),
    );

  let getResults = query =>
    /*
     * This is similar to `open Json.Decode`, it allows the Promise functions
     * to be available within the parentheses
     */
    Promise.(
      Fetch.fetch("https://api.github.com/search/repositories?q=" ++ query)
      |> then_(Fetch.Response.json)
      |> then_(json => decodeResults(json) |> resolve)
      |> then_(results =>
           results
           |> List.filter(optionalItem =>
                switch (optionalItem) {
                | Some(_) => true
                | None => false
                }
              )
           /* Turn our items out of option types into a regular record */
           |> List.map(item =>
                switch (item) {
                | Some(item) => item
                }
              )
           |> resolve
         )
    );
};

/*
 Pour avoir un stateful component :
 1 - il faut rajouter un initialState sous forme d'une function
 qui retourne le record avec les valeurs par default
 2 - il faut rajouter le reducer, un peu comme redux en utilisant le pattern-matching
 3 - modification du render, remplacer _self par self nb : c'est comme le this
 */
let make = _children => {
  ...component,
  initialState: () => {input: "", isLoading: false, results: []},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(newInput) =>
      ReasonReact.Update({...state, input: newInput})
    | UpdateResults(results) =>
      ReasonReact.Update({...state, isLoading: false, results})
    | Search =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isLoading: true},
        (
          self => {
            let value = self.state.input;
            /* This function needs to return a "unit" type, wo we'll insert it here */
            Js.log("this is reason, result : " ++ value);
            let _ =
              Api.getResults(value)
              |> Js.Promise.then_(results => {
                   self.send(UpdateResults(results));
                   Js.Promise.resolve();
                 });
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
            ReasonReact.string("Loading...") :
            self.state.results
            /* Convert to list to an array for ReasonReact's type bindings */
            |> Array.of_list
            /* Map each array item to a <Card /> component */
            |> Array.map(({name, href, description}) =>
                 <Card key=name name href description />
               )
            /* Transform the array into a valid React node, similar to ReasonReact.string */
            |> ReasonReact.array
        }
      </div>
    </div>,
};