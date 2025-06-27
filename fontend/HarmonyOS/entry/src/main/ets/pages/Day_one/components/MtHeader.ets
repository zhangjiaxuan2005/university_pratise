@Component
 export struct MtHeader {
  build() {
    Row() {
      Text("我是Header")
        .fontSize(24)
    }
    .justifyContent(FlexAlign.Center)
    .width("100%")
    .height(54)
    .backgroundColor(Color.Yellow)
  }
}