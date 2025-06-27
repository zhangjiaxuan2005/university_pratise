import { MtFooter } from './components/MtFooter'
import { MtHeader } from './components/MtHeader'
import { MtMiddle } from './components/MtMiddle'

@Entry
@Component
struct MtExample {
  build() {
    Column() {
      MtHeader()
      MtMiddle()
      MtFooter()
    }

    .width("100%")
    .height("100%")
    .backgroundColor(Color.Pink)
  }
}