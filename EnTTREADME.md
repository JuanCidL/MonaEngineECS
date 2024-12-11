# Intrducción

El presente archivo busca dar a conocer sobre la implementación realizada de la librería EnTT a el motor de juegos MonaEngine.

# Solución

Con la finalidad de tener un sistema funcional, se propuso una solución que va en línea con lo discutido con el profesor. La solución incorporada tiene un MetaWorld que maneja el intercambio de información entre el World de MonaEngine y el EnTTWorld implementado por nosotros. Para esto, se tuveron que desarrollar las clases de EnTTWorld, EnTTEventManager, MetaWorld y EnTTTransitionHandler, que buscan mantener la comunicación adecuada entre el World y el EnttWorld.

El EnTTWorld busca replicar lo que hace el World de Mona, siendo una especie de SceneManager. 

Para esta entrega se agregaron dos sistemas que se encargan del manejo de componentes: el EnttAudioSystem y el RenderingSystem. En ambos casos, estos intentan de replicar lo que hacen el AudioSystem y Rendering, respectivamente, pero adpatando las funcionalidad a una arquitectura ECS como lo sería EnTT. Es por esto que todo el manejo de componentes en este sistema se realiza mediante las clases EnTTCompnentManager y EnTTEventManager, que hacen una especie de wrapper a EnTT y el manejo de eventos diseñado. 

Utilizar EnTT facilitó mucho la lectura del código y el acceso a compnentes, haciendo que la estructura de ComponentManager de Mona no sea necesaria. 

A pesar de esto, la solución entragada presenta un problema en que no compila debido a que no reconoce el método OnWindowResizeEvent como Member Function en EnttEventManager. Debido a esto, no pudimos comprobar la efectividad de nuestra solución ni mostrar el trabajo realizado.

Es importante destacar que todo el código realizado en torno a MetaWrold se hizo considerando que esta es una herramienta que un desarrollador de juegos podrá utilizar.