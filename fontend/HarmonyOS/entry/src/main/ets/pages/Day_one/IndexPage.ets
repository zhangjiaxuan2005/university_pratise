import { promptAction } from '@kit.ArkUI'

@Entry
@Component
struct indexPages {
  build() {
    Column({ space: 2 }) {
      Text("Hello HarmonyOS")
        .width(200)
        .height(100)
        .fontSize(24)
        .fontColor(Color.Red)
        .backgroundColor(Color.Orange)
        .onClick(() => {
          promptAction.showToast({
            message: 'Welcome'
          })
        })
      Button("welcome")
        .width(100)
        .height(50)
        .onClick(()=>{
          promptAction.showDialog({
            message:"Hello HarmonyOS"
          })
        })
    }
    .width("100%")
  }
}