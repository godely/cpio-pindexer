{
  "targets": [
    {
      "target_name": "pindexer",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "pindexer.cpp", "partialname.h", "problem.h" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
