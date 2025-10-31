// swift-tools-version: 5.9
import PackageDescription

let package = Package(
    name: "PlotterCLI",
    platforms: [
        .macOS(.v13)
    ],
    products: [
        .executable(
            name: "plotter",
            targets: ["PlotterCLI"]
        ),
    ],
    dependencies: [
        .package(url: "https://github.com/apple/swift-argument-parser.git", from: "1.3.0"),
    ],
    targets: [
        .target(
            name: "PlotterObjCBridge",
            dependencies: [],
            publicHeadersPath: "include",
            cxxSettings: [
                .unsafeFlags([
                    "-I", "/Users/tlee/Developer/Plotter/PlotterEntities/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterUseCases/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterRepositories/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterDataSourceRouter/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterDTOs/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterSqliteDTOs/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterSqliteMappers/include",
                    "-I", "/Users/tlee/Developer/Plotter/PlotterSqliteDataSource/include",
                ]),
            ],
            linkerSettings: [
                .unsafeFlags([
                    "-L", "/Users/tlee/Developer/Plotter/PlotterLogger/build",
                    "-L", "/Users/tlee/Developer/Plotter/PlotterEntities/build/lib",
                    "-L", "/Users/tlee/Developer/Plotter/PlotterSqliteDTOs/build",
                    "-L", "/Users/tlee/Developer/Plotter/PlotterSqliteMappers/build",
                    "-L", "/Users/tlee/Developer/Plotter/PlotterSqliteDataSource/build",
                    "-L", "/Users/tlee/Developer/Plotter/PlotterUseCases/build/lib",
                    "-lPlotterLogger",
                    "-lNoteTaker",
                    "-lPlotterSqliteDTOs",
                    "-lPlotterSqliteMappers",
                    "-lPlotterSqliteDataSource",
                    "-lPlotterUseCases",
                ]),
                .linkedLibrary("sqlite3"),
                .linkedLibrary("c++"),
            ]
        ),
        .executableTarget(
            name: "PlotterCLI",
            dependencies: [
                "PlotterObjCBridge",
                .product(name: "ArgumentParser", package: "swift-argument-parser"),
            ]
        ),
    ],
    cxxLanguageStandard: .cxx17
)
