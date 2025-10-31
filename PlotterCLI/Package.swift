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
                    "-I", "../PlotterEntities/include",
                    "-I", "../PlotterUseCases/include",
                    "-I", "../PlotterRepositories/include",
                    "-I", "../PlotterDataSourceRouter/include",
                    "-I", "../PlotterDTOs/include",
                    "-I", "../PlotterSqliteDTOs/include",
                    "-I", "../PlotterSqliteMappers/include",
                    "-I", "../PlotterSqliteDataSource/include",
                    "-I", "../PlotterFilesystemDataSource/include",
                ]),
            ],
            linkerSettings: [
                .unsafeFlags([
                    "-L", "../PlotterLogger/build",
                    "-L", "../PlotterEntities/build/lib",
                    "-L", "../PlotterSqliteDTOs/build",
                    "-L", "../PlotterSqliteMappers/build",
                    "-L", "../PlotterSqliteDataSource/build",
                    "-L", "../PlotterUseCases/build/lib",
                    "-L", "../PlotterFilesystemDataSource/build",
                    "-lPlotterLogger",
                    "-lNoteTaker",
                    "-lPlotterSqliteDTOs",
                    "-lPlotterSqliteMappers",
                    "-lPlotterSqliteDataSource",
                    "-lPlotterUseCases",
                    "-lPlotterFilesystemDataSource",
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
