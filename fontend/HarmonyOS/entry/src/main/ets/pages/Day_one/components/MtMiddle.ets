@Component
export struct MtMiddle {
  build() {
    Column() {
      Text("我是Middle")
        .fontSize(24)

    }
    .justifyContent(FlexAlign.Center)
    .layoutWeight(1)
    .width("100%")
    .backgroundColor(Color.Grey)
  }
}