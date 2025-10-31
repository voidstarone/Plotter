# PlotterDTOs

This package defines the base Data Transfer Object (DTO) contracts for the Plotter application.

## Purpose

- Defines base DTO structures that all data source implementations must inherit from
- Provides a common interface that allows repositories to work with different data sources
- Maintains clean architecture by separating data contracts from entities and business logic

## Architecture

```
PlotterDTOs (base contracts)
     ↑
     |
     +-- PlotterSqliteDataSource (SqliteNoteDTO : NoteDTO)
     |
     +-- OtherDataSource (OtherNoteDTO : NoteDTO)
     
PlotterRepositories depends on PlotterDTOs to convert DTOs ↔ Entities
```

## Usage

Data source implementations should create DTOs that inherit from the base types:

```cpp
#include "BaseDTOs.h"

namespace my_datasource {
    struct MyNoteDTO : public plotter::dto::NoteDTO {
        // Add any additional fields specific to your data source
        std::string extraField;
    };
}
```
