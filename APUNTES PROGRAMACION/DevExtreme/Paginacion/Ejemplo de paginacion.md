# Ejemplo de paginación

## Creamos un DTO
~~~csharp
namespace WebPortalClientesReact.DTOs
{
    public class PeticionHistoricosDTO
    {
        public string Prefijo {  get; set; }    // Para saber que prefijo de tabla va a coger (sms, ia, sig, ...)
        public int Id { get; set; }             // Id de la empresa a consultar

        // Parámetros de DevExtreme
        public int Skip { get; set; } = 0;
        public int Take { get; set; } = 15;
        public List<SortDescriptor>? Sort { get; set; } // En C# hay que crear el SortDescriptor
        public object? Filter { get; set; }

    }

    public class SortDescriptor  // Para ordenar la consulta
    {
        public string Selector { get; set; } = string.Empty;    // Nombre de la columna que queremos seleccionar
        public bool Desc { get; set; } = false;                 // Si es desc (true) o ascendente (false)
    }
}
~~~
- Cuando se haga la petición desde el front, llegaría algo así:
    ~~~js
    {
        "Prefijo": "sms_envios_",
        "Id": 123,
        "Skip": 0,
        "Take": 15,
        "Sort": [{"Selector":"fecha","Desc":true}],
        "Filter": null
    }
    ~~~

## Integrar los parámetros que nos envía DevExtreme (`Skip`, `Take`, `Sort` y `Filter`)
- Se integran los parámetros de DevExtreme en el backend para que la consulta que ejecuta Dapper haga:
    - Paginación
    - Odenación
    - Filtrado
- Así evitamos traer todo y filtrar en el frontend

## Modificamos el `Controller` para que reciba el DTO completo
- Así podemos pasar todos los parámetros que DevExtreme enviará (Skip, Take, Sort, Filter)
    ~~~csharp
    [HttpPost]
    [Route("Historicos")]
    public async Task<IActionResult> ObtenerHistoricosAsync([FromBody] HistoricosRequest request)
    ~~~

## Modificamos el `Service` para que reciba el DTO completo