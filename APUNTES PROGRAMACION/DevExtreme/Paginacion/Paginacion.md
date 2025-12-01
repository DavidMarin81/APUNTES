# Paginación con DevExtreme

## Idea general
- Hacer que DevExtreme le diga al backend que datos tiene que traer de la base de datos
- Para ello usamos `remoteOperations: true` en DevExtreme
- La idea de esto es que el filtro, orden y paginación se haga en el backend, y el frontend reciba solo los datos necesarios
- Al resetear filtros, DevExtreme pida todos los registros y no se pierda información

## Cómo lograrlo
- Para lograr ésto, necesitamos cambios en las tres capas:
    - `DTO`  de petición en el backend para recibir filtros, orden y paginación
    - `Service` que aplique el filtro y la paginación (o los traduzca a SQL)
    - `Frontend` que use un `DataSource` custom en DevExtreme para enviar los parámetros al backend

## Crear un DTO
- Se crea un DTO con los parámetros de DevExtreme
    ~~~csharp
    public class HistoricosRequest
    {
        public string Prefijo { get; set; }
        public int Id { get; set; }

        // Parámetros DevExtreme
        public int Skip { get; set; } = 0;
        public int Take { get; set; } = 15;
        public object Sort { get; set; } // DevExtreme envía un array con { selector, desc }
        public object Filter { get; set; } // Array con filtros
    }
    ~~~
## Cómo DevExtreme gestiona los parámetros
- `take` (cuántos registros por página)
    - Se define a través del `pageSize` de `Paging` o `defaultPageSize`
        ~~~jsx
            <Paging defaultPageSize={15} />   // DevExtreme "sabe" que cada página debe traer 15 registros
        ~~~
    - `take = 15` -> DevExtreme lo envía automáticamente al backend
- `skip` (cuántos registros saltarse)
    - No se define directamente
    - DevExtreme calcula `skip` automáticamente según la página que el usuario elija:

        |Página|skip|take|
        |---|---|---|
        |1|0|15|
        |2|15|15|
        |3|30|15|

    - Esto se hace cuando el usuario pulsa en otra página del pager o cambia `pageSize`

- `sort` (orden)
    - Se activa si el usuario hace click en el encabezado de una columna o si quieres un orden inicial
    - Para un orden inicial, puedes usar `defaultSort` o `sortBy` 
        ~~~js
        <DataGrid
            defaultSort={[{ selector: "fecha", desc: true }]}
        >
        ~~~
    - Después de eso, si el usuario cambia la columna, DevExtreme actualiza automáticamente `sort`

- `filter` (filtro)
    - Se activa si tienes `FilterRow`o `HeaderFilter` visibles
        ~~~js
        <FilterRow visible={true} />
        <HeaderFilter visible={true} />
        ~~~
    - Para un filtro inicial programático (por ejemplo últimos 15 días)
        ~~~js
        const ultimos15Dias = ["fecha", ">=", new Date(Date.now() - 15*24*60*60*1000)];

        <DataGrid
            defaultFilterValue={ultimos15Dias}
        />
        ~~~
    - DevExtreme envía esto al backend en la primera carga
    - Si el usuario cambia el filtro, DevExtreme recalcula `filter` y lo envía de nuevo

- Resumen clave
    - Nunca asignas `skip` ni `take` manualmente en la tabla
    - `sort` y `filter` solo se configuran si quieres valores iniciales
    - DevExtreme envía automáticamente todo al backend cada vez que cambia algo (página, filtro, orden)
- Ejemplo de un Datagrid con estos parámetros:
    ~~~js
    import React from "react";
    import { DataGrid, Column, FilterRow, HeaderFilter, Paging, Pager, Export } from "devextreme-react/data-grid";

    function TablaEjemplo({ datos }) {
        // Filtro inicial: últimos 15 días
        const fechaActual = new Date();
        const quinceDiasAtras = new Date();
        quinceDiasAtras.setDate(fechaActual.getDate() - 15);
        const filtroInicial = ["fecha", ">=", quinceDiasAtras];

        // Orden inicial: por fecha descendente
        const ordenInicial = [{ selector: "fecha", desc: true }];

        return (
            <DataGrid
            dataSource={datos}                  // Aquí vendrían los datos del backend
            keyExpr="idUnico"                   // Identificador único de cada fila
            remoteOperations={true}             // Activa skip, take, sort y filter en backend
            defaultFilterValue={filtroInicial}  // Filter inicial
            defaultSort={ordenInicial}          // Sort inicial
            columnAutoWidth={true}
            allowColumnResizing={true}
            wordWrapEnabled={true}
            showBorders={true}
            >
            {/* Filtros visibles en columnas */}
            <FilterRow visible={true} />                // La fila de filtros que aparece justo debajo del encabezado de cada columna
            <HeaderFilter visible={true} />             // Un filtro desplegable que aparece en el encabezado de cada columna

            {/* Paginación */}
            <Paging defaultPageSize={15} />             // Configuración de cuántos registros se muestran por página en la tabla  
            <Pager visible={true} showInfo={true} />    // El control visual de paginación que aparece normalmente en la parte inferior de la tabla

            {/* Export a Excel */}
            <Export enabled={true} fileName="export" />

            {/* Columnas */}
            <Column dataField="fecha" caption="Fecha" dataType="date" format="dd/MM/yyyy" />
            <Column dataField="estadoEnvio" caption="Estado" />
            <Column dataField="remitente" caption="Remitente" />
            <Column dataField="destino" caption="Destinatario" />
            </DataGrid>
        );
    }

    export default TablaEjemplo;

    ~~~

## Explicación de los parámetros
- Estos parámetros permiten al backend saber exactamente qué datos necesita DevExtreme, sin enviar todo
    - `Skip` y `Take` sirven para paginación
        - `Skip` dice cuantos registros debe saltarse DevExtreme
        - `Take` cuantos registros debe devolver
        - Como esto conseguimos que DevExtreme haga paginación. Si ponemos
            ~~~json
            {
                "skip": 0,
                "take": 15
            }
            ~~~
            - Cuando pulsemos en el paginador 1 de la tabla, DevExtreme nos traerá los 15 primeros registros.
            - Si luego pulsamos en el 3, DevExtreme se saltará 2 bloques de 15 resultados y nos mostrará los resultados del 30 al 45
            - DexExtreme se encarga de hacer los cálculos para hacer la paginación que necesitamos
    - `Sort` es un array con `{ selector: "fecha", desc: true }` u otras columnas, indica el orden que el usuario ha seleccionado
        ~~~json
        {
            "sort": [
                {
                "selector": "fecha",
                "desc": true
                }
            ]
        }
        ~~~
        - "selector": "fecha" -> ordena por la columna "fecha"
        - "desc": true -> orden descendente (con false, el orden es ascendente)
        - Puede ordenar varias columnas
            ~~~json
            [
                { "selector": "fecha", "desc": true },
                { "selector": "nombre", "desc": false }
            ]
            ~~~
            - Ordena por fecha descendente
            - Si hay empate, ordena por nobre ascendente

    - `Filter` es un array con filtros activos, por ejemplo: `[["fecha", ">=", "2025-10-01"]]`
        - DevExtreme envía un `array` con las condiciones de filtrado
        - Formato básico: ["columna"j, "operador", valor]
        - Operadores: =, >, >=, <, <=, `contains`, ...
        - Para combinar condiciones: usar `"and"` `"or"` dentro del array
        - El backend debe traducirlo a consultas SQL o LINQ
        - Orden correcto:
            - Filtrar primero
            - Aplicar sort
            - Aplicar skip y take
        - Ejemplo:
            ~~~json
            [
                ["estadoEnvio", "=", "Pendiente/Error"],
                "or",
                [
                    ["fecha", ">", "2025-11-05"],
                    "and",
                    ["destino", "=", "Juan"]
                ]
            ]
            ~~~
            - Estado = "Pendiente/Error"
            - O (fecha > 15 días y destino = Juan)
    - Ejemplo completo de flujo
        - El usuario quiere ver últimos 15 días
        - DevExtreme envía
            ~~~json
            {
                "filter": ["fecha", ">", "2025-11-05"],
                "sort": [{ "selector": "fecha", "desc": true }],
                "skip": 0,
                "take": 15
            }
             ~~~
